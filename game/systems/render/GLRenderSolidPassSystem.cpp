#include "GLRenderSolidPassSystem.h"

#include <glad/glad.h>
#include <spdlog/spdlog.h>
#include <glm/gtc/matrix_transform.hpp>
#include <magic_enum.hpp>

#include "core/components/TransformComponent.h"
#include "game/components/display/DisplayComponent.h"
#include "game/components/render/GLMeshComponent.h"
#include "game/components/render/GLRenderPassData.h"
#include "game/components/render/GLTextureComponent.h"
#include "game/components/tags/InvisibleMeshTag.h"
#include "game/utils/GLRenderUtils.h"

//
// ISystem
//

void GLRenderSolidPassSystem::OnCreate(registry_t & Registry_)
{
  auto & RenderData = QueryOrCreate<TGLRenderPassData>(Registry_);
  auto & Display    = QuerySingle<TDisplayComponent>(Registry_);

  glGenFramebuffers(1, &m_FBO);

  glGenTextures(1, &RenderData.SolidTexture);
  glBindTexture(GL_TEXTURE_2D, RenderData.SolidTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, static_cast<GLsizei>(Display.Width), static_cast<GLsizei>(Display.Height), 0, GL_RGBA, GL_HALF_FLOAT, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBindTexture(GL_TEXTURE_2D, 0);

  glGenTextures(1, &RenderData.CompositeDepthTexture);
  glBindTexture(GL_TEXTURE_2D, RenderData.CompositeDepthTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, static_cast<GLsizei>(Display.Width), static_cast<GLsizei>(Display.Height), 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
  glBindTexture(GL_TEXTURE_2D, 0);

  glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, RenderData.SolidTexture, 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, RenderData.CompositeDepthTexture, 0);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    spdlog::critical("!!! ERROR Opaque framebuffer is not complete !!!");

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  RenderData.SolidFBO = m_FBO;
}

void GLRenderSolidPassSystem::OnUpdate(registry_t & Registry_, float Delta_)
{
  glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
  glClearColor(0.52f, 0.807f, 0.92f, 1.f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  RenderMeshes(Registry_, Registry_.view<TGLSolidMeshComponent, TGLShaderComponent, TTransformComponent>().each());
}

void GLRenderSolidPassSystem::OnDestroy(registry_t & Registry_)
{
  // Empty
}
