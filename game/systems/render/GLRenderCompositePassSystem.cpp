#include "GLRenderCompositePassSystem.h"

#include <spdlog/spdlog.h>

#include "game/components/render/GLRenderPassData.h"
#include "game/resources/ShaderLibrary.h"

//
// ISystem
//

void GLRenderCompositePassSystem::OnCreate(registry_t & Registry_)
{
  m_CompositeShader = CShaderLibrary::Load("res/shaders/composite_shader");

  if (!m_CompositeShader.IsValid())
    spdlog::critical("!!! ERROR: Failed to load composite shader !!!");

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
}

void GLRenderCompositePassSystem::OnUpdate(registry_t & Registry_, float Delta_)
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
  glBindVertexArray(m_VAO);
  glDrawArrays(GL_TRIANGLES, 0, 6);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  RenderData.ScreenTexture = RenderData.SolidTexture;
}

void GLRenderCompositePassSystem::OnDestroy(registry_t & Registry_)
{
  // Empty
}
