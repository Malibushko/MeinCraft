#include "GLRenderPostEffectsPassSystem.h"

#include <spdlog/spdlog.h>

#include "game/components/display/DisplayComponent.h"
#include "game/components/render/GLRenderPassData.h"
#include "game/resources/ShaderLibrary.h"

//
// ISystem
//

void GLRenderPostEffectsPassSystem::OnCreate(registry_t & Registry_)
{
  auto & Display = QuerySingle<TDisplayComponent>(Registry_);

  m_Shader = CShaderLibrary::Load("res/shaders/posteffects_shader");

  if (!m_Shader.IsValid())
    spdlog::error("Failed to load post effects shader");

  glGenFramebuffers(1, &m_FBO);
  glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

  glGenTextures(1, &m_Texture);
  glBindTexture(GL_TEXTURE_2D, m_Texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, static_cast<GLsizei>(Display.Width), static_cast<GLsizei>(Display.Height), 0, GL_RGBA, GL_FLOAT, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_Texture, 0);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    spdlog::error("Posteffects framebuffer not complete!");

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

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

void GLRenderPostEffectsPassSystem::OnUpdate(registry_t & Registry_, float Delta_)
{
  auto & RenderData = QuerySingle<TGLRenderPassData>(Registry_);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glClearColor(0.f, 0.f, 0.f, 0.f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

  glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
  glUseProgram(m_Shader.ShaderID);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, RenderData.ScreenTexture);

  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, RenderData.SSAOTexture);

  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, RenderData.BloomTexture);

  glBindVertexArray(m_ScreenQuadVAO);
  glDrawArrays(GL_TRIANGLES, 0, 6);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glUseProgram(0);

  RenderData.ScreenTexture = m_Texture;
}

void GLRenderPostEffectsPassSystem::OnDestroy(registry_t & Registry_)
{
  // Empty
}
