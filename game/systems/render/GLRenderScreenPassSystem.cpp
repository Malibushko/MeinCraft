#include "GLRenderScreenPassSystem.h"

#include <spdlog/spdlog.h>

#include "game/components/render/GLRenderPassData.h"
#include "game/resources/ShaderLibrary.h"

//
// ISystem
//

void GLRenderScreenPassSystem::OnCreate(registry_t & Registry_)
{
  m_CompositeShader = CShaderLibrary::Load("res/shaders/composite_shader");
  m_ScreenShader    = CShaderLibrary::Load("res/shaders/screen_shader");

#ifdef DEBUG_DEPTH

  m_DebugDepthShader = CShaderLibrary::Load("res/shaders/debug_depth_shader");

#endif

  if (!m_CompositeShader.IsValid() || !m_ScreenShader.IsValid())
    spdlog::critical("!!! ERROR: Failed to load composite or screen shader !!!");

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

void GLRenderScreenPassSystem::OnUpdate(registry_t & Registry_, float Delta_)
{
  auto & RenderData = QuerySingle<TGLRenderPassData>(Registry_);

  // Draw composite image
  glDepthFunc(GL_ALWAYS);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glBindFramebuffer(GL_FRAMEBUFFER, RenderData.SolidFBO);

  glUseProgram(m_CompositeShader.ShaderID);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, RenderData.AccumulatorTexture);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, RenderData.RevealTexture);
  glBindVertexArray(m_ScreenQuadVAO);
  glDrawArrays(GL_TRIANGLES, 0, 6);

  // Finally draw to backbuffer
  glDisable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE);
  glDisable(GL_BLEND);

  // Bind backbuffer
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glClearColor(0.f, 0.f, 0.f, 0.f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

#ifdef DEBUG_DEPTH

  glUseProgram(m_DebugDepthShader.ShaderID);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, RenderData.DepthTexture);

#else

  glUseProgram(m_ScreenShader.ShaderID);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, RenderData.SolidTexture);

#endif

  glBindVertexArray(m_ScreenQuadVAO);
  glDrawArrays(GL_TRIANGLES, 0, 6);
}

void GLRenderScreenPassSystem::OnDestroy(registry_t & Registry_)
{
  // Empty
}
