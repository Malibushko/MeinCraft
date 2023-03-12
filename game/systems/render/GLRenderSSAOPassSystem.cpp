#include "GLRenderSSAOPassSystem.h"

#include <random>
#include <spdlog/spdlog.h>

#include "game/components/display/DisplayComponent.h"
#include "game/components/render/GLRenderPassData.h"
#include "game/resources/ShaderLibrary.h"
#include "game/utils/NumericUtils.h"

static constexpr int SSAO_KERNEL_SHADER_LOCATION = 0;

//
// ISystem
//

void GLRenderSSAOPassSystem::OnCreate(registry_t & Registry_)
{
  auto & Display = QuerySingle<TDisplayComponent>(Registry_);

  glGenFramebuffers(1, &m_FBO);
  glGenFramebuffers(1, &m_BlurFBO);
  glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

  glGenTextures(1, &m_SSAOTexture);
  glBindTexture(GL_TEXTURE_2D, m_SSAOTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, static_cast<GLsizei>(Display.Width), static_cast<GLsizei>(Display.Height), 0, GL_RED, GL_FLOAT, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_SSAOTexture, 0);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    spdlog::error("SSAO Framebuffer not complete!");

  glBindFramebuffer(GL_FRAMEBUFFER, m_BlurFBO);
  glGenTextures(1, &m_SSAOBlurTexture);
  glBindTexture(GL_TEXTURE_2D, m_SSAOBlurTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, static_cast<GLsizei>(Display.Width), static_cast<GLsizei>(Display.Height), 0, GL_RED, GL_FLOAT, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_SSAOBlurTexture, 0);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    spdlog::error("SSAO Blur Framebuffer not complete!");

  const std::uniform_real_distribution<GLfloat> FloatsGenerator(0.0, 1.0);
  std::default_random_engine Engine;

  for (unsigned int i = 0; i < 64; ++i)
  {
    glm::vec3 Sample(FloatsGenerator(Engine) * 2.0 - 1.0, FloatsGenerator(Engine) * 2.0 - 1.0, FloatsGenerator(Engine));

    Sample = glm::normalize(Sample);
    Sample *= FloatsGenerator(Engine);

    float Scale = float(i) / 64.0f;

    // scale samples s.t. they're more aligned to center of kernel
    Scale = Utils::Lerp(0.1f, 1.0f, Scale * Scale);

    Sample *= Scale;

    m_Kernel.push_back(Sample);
  }

  std::vector<glm::vec3> SSAONoise;
  for (unsigned int i = 0; i < 16; i++)
  {
    glm::vec3 Noise(FloatsGenerator(Engine) * 2.0 - 1.0, FloatsGenerator(Engine) * 2.0 - 1.0, 0.0f); // rotate around z-axis (in tangent space)

    SSAONoise.push_back(Noise);
  }

  glGenTextures(1, &m_NoiseTexture);
  glBindTexture(GL_TEXTURE_2D, m_NoiseTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 4, 4, 0, GL_RGB, GL_FLOAT, &SSAONoise[0]);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  m_SSAOShader     = CShaderLibrary::Load("res/shaders/ssao_shader");
  m_SSAOBlurShader = CShaderLibrary::Load("res/shaders/ssao_blur_shader");

  if (!m_SSAOShader.IsValid() || !m_SSAOBlurShader.IsValid())
    spdlog::error("Failed to load SSAO shaders!");

  glUseProgram(m_SSAOShader.ShaderID);
  glUniform3fv(SSAO_KERNEL_SHADER_LOCATION, m_Kernel.size(), &m_Kernel[0][0]);
  glUseProgram(0);

  const float ScreenQuadVertices[] = {
    // positions		    // uv
    -1.0f, -1.0f, 0.0f,	0.0f, 0.0f,
     1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
     1.0f,  1.0f, 0.0f, 1.0f, 1.0f,

     1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
    -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
    -1.0f, -1.0f, 0.0f, 0.0f, 0.0f
  };

  GLuint QuadVBO;

  glGenVertexArrays(1, &m_ScreenQuadVAO);
  glGenBuffers(1, &QuadVBO);

  glBindVertexArray(m_ScreenQuadVAO);
  glBindBuffer(GL_ARRAY_BUFFER, QuadVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(ScreenQuadVertices), ScreenQuadVertices, GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
  glBindVertexArray(0);
}

void GLRenderSSAOPassSystem::OnUpdate(registry_t & Registry_, float Delta_)
{
  auto & RenderData = QuerySingle<TGLRenderPassData>(Registry_);

  glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
  glClearColor(0.f, 0.f, 0.f, 0.f);
  glClear(GL_COLOR_BUFFER_BIT);

  glUseProgram(m_SSAOShader.ShaderID);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, RenderData.PositionsTexture);

  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, RenderData.NormalTexture);

  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, m_NoiseTexture);

  glBindVertexArray(m_ScreenQuadVAO);
  glDrawArrays(GL_TRIANGLES, 0, 6);

  // Blur

  glBindFramebuffer(GL_FRAMEBUFFER, m_BlurFBO);
  glClearColor(0.f, 0.f, 0.f, 0.f);
  glClear(GL_COLOR_BUFFER_BIT);

  glUseProgram(m_SSAOBlurShader.ShaderID);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, m_SSAOTexture);

  glBindVertexArray(m_ScreenQuadVAO);
  glDrawArrays(GL_TRIANGLES, 0, 6);

  glUseProgram(0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  RenderData.SSAOTexture = m_SSAOBlurTexture;
}

void GLRenderSSAOPassSystem::OnDestroy(registry_t & Registry_)
{
  // Empty
}
