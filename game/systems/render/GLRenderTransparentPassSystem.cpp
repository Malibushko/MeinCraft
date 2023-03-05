#include "GLRenderTransparentPassSystem.h"

#include <spdlog/spdlog.h>

#include "core/components/TransformComponent.h"
#include "game/components/display/DisplayComponent.h"
#include "game/components/render/GLMeshComponent.h"
#include "game/components/render/GLRenderPassData.h"
#include "game/utils/GLRenderUtils.h"

//
// ISystem
//

void GLRenderTransparentPassSystem::OnCreate(registry_t & Registry_)
{
  auto & RenderData = QueryOrCreate<TGLRenderPassData>(Registry_);
  auto & Display    = QuerySingle<TDisplayComponent>(Registry_);

  glGenFramebuffers(1, &m_FBO);

  glGenTextures(1, &RenderData.AccumulatorTexture);
  glBindTexture(GL_TEXTURE_2D, RenderData.AccumulatorTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, static_cast<GLsizei>(Display.Width), static_cast<GLsizei>(Display.Height), 0, GL_RGBA, GL_HALF_FLOAT, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBindTexture(GL_TEXTURE_2D, 0);

  glGenTextures(1, &RenderData.RevealTexture);
  glBindTexture(GL_TEXTURE_2D, RenderData.RevealTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, static_cast<GLsizei>(Display.Width), static_cast<GLsizei>(Display.Height), 0, GL_RED, GL_FLOAT, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBindTexture(GL_TEXTURE_2D, 0);

  glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, RenderData.AccumulatorTexture, 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, RenderData.RevealTexture, 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,  RenderData.CompositeDepthTexture, 0);

  const GLenum TransparentDrawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
  glDrawBuffers(2, TransparentDrawBuffers);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    spdlog::critical("!!! ERROR: Transparent framebuffer is not complete !!!");

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  RenderData.TransparentFBO = m_FBO;
}

void GLRenderTransparentPassSystem::OnUpdate(registry_t & Registry_, float Delta_)
{
  static constexpr glm::vec4 ZeroFillVector(0.0f);
  static constexpr glm::vec4 OneFillVector(1.0f);

  glDepthMask(GL_FALSE);
  glEnable(GL_BLEND);
  glBlendFunci(0, GL_ONE, GL_ONE);
  glBlendFunci(1, GL_ZERO, GL_ONE_MINUS_SRC_COLOR);
  glBlendEquation(GL_FUNC_ADD);

  glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
  glClearBufferfv(GL_COLOR, 0, &ZeroFillVector[0]);
  glClearBufferfv(GL_COLOR, 1, &OneFillVector[0]);

  RenderMeshes(Registry_, Registry_.view<TGLTranslucentMeshComponent, TGLShaderComponent, TTransformComponent>().each());
}

void GLRenderTransparentPassSystem::OnDestroy(registry_t & Registry_)
{
  // Empty
}
