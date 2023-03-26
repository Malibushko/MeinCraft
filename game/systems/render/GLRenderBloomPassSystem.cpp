#include "GLRenderBloomPassSystem.h"

#include <spdlog/spdlog.h>

#include "core/components/TransformComponent.h"
#include "game/components/display/DisplayComponent.h"
#include "game/components/render/GLMeshComponent.h"
#include "game/components/render/GLRenderPassData.h"
#include "game/components/tags/InvisibleMeshTag.h"
#include "game/resources/ShaderLibrary.h"
#include "game/utils/GLRenderUtils.h"

static constexpr int BLOOM_BLUR_SHADER_PASS_COUNT               = 10;
static constexpr int BLOOM_BLUR_SHADER_HORIZONTAL_FLAG_LOCATION = 0;

//
// ISystem
//

void GLRenderBloomPassSystem::OnCreate(registry_t & Registry_)
{
  auto & RenderData = QueryOrCreate<TGLRenderPassData>(Registry_);
  auto & Display    = QuerySingle<TDisplayComponent>(Registry_);

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

  glGenVertexArrays(1, &m_VAO);
  glGenBuffers(1, &QuadVBO);

  glBindVertexArray(m_VAO);
  glBindBuffer(GL_ARRAY_BUFFER, QuadVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(ScreenQuadVertices), ScreenQuadVertices, GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
  glBindVertexArray(0);

  glGenFramebuffers(1, &m_FBO);
  glGenTextures(1, &RenderData.BloomTexture);
  glBindTexture(GL_TEXTURE_2D, RenderData.BloomTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, Display.Width, Display.Height, 0, GL_RGBA, GL_FLOAT, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, RenderData.BloomTexture, 0);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    spdlog::critical("!!! ERROR: Bloom framebuffer is not complete !!!");

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  m_Shader     = CShaderLibrary::Load("res/shaders/bloom_shader");
  m_BlurShader = CShaderLibrary::Load("res/shaders/bloom_blur_shader");

  if (!m_Shader.IsValid() || !m_BlurShader.IsValid())
    spdlog::critical("!!! ERROR: Bloom shader is not valid !!!");
}

void GLRenderBloomPassSystem::OnUpdate(registry_t & Registry_, float Delta_)
{
  auto & RenderData = QueryOrCreate<TGLRenderPassData>(Registry_);

  glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

  glUseProgram(m_Shader.ShaderID);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, RenderData.ScreenTexture);
  glBindVertexArray(m_VAO);
  glDrawArrays(GL_TRIANGLES, 0, 6);

  bool Horizontal = false;

  glUseProgram(m_BlurShader.ShaderID);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, RenderData.BloomTexture);

  for (int i = 0; i < BLOOM_BLUR_SHADER_PASS_COUNT; i++)
  {
    glUniform1i(BLOOM_BLUR_SHADER_HORIZONTAL_FLAG_LOCATION, Horizontal);

    glBindVertexArray(m_VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    Horizontal = !Horizontal;
  }

  glUseProgram(0);
}

void GLRenderBloomPassSystem::OnDestroy(registry_t & Registry_)
{
  // Empty
}
