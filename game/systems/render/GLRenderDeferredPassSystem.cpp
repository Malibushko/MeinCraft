#include "GLRenderDeferredPassSystem.h"

#include <spdlog/spdlog.h>

#include "GLRenderTransparentPassSystem.h"
#include "core/components/TransformComponent.h"
#include "game/components/display/DisplayComponent.h"
#include "game/components/render/GLMeshComponent.h"
#include "game/components/render/GLRenderPassData.h"
#include "game/components/tags/InvisibleMeshTag.h"
#include "game/resources/ShaderLibrary.h"
#include "game/utils/GLRenderUtils.h"

//
// ISystem
//

void GLRenderDeferredPassSystem::OnCreate(registry_t & Registry_)
{
  auto & RenderData = QueryOrCreate<TGLRenderPassData>(Registry_);
  auto & Display    = QuerySingle<TDisplayComponent>(Registry_);

  glGenFramebuffers(1, &m_FBO);
  glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

  glGenTextures(1, &RenderData.PositionTexture);
  glBindTexture(GL_TEXTURE_2D, RenderData.PositionTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, Display.Width, Display.Height, 0, GL_RGBA, GL_FLOAT, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, RenderData.PositionTexture, 0);

  glGenTextures(1, &RenderData.NormalTexture);
  glBindTexture(GL_TEXTURE_2D, RenderData.NormalTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, Display.Width, Display.Height, 0, GL_RGBA, GL_FLOAT, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, RenderData.NormalTexture, 0);

  glGenTextures(1, &RenderData.AlbedoTexture);
  glBindTexture(GL_TEXTURE_2D, RenderData.AlbedoTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Display.Width, Display.Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, RenderData.AlbedoTexture, 0);

  unsigned int Attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
  glDrawBuffers(3, Attachments);

  unsigned int Depth;
  glGenRenderbuffers(1, &Depth);
  glBindRenderbuffer(GL_RENDERBUFFER, Depth);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, Display.Width, Display.Height);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, Depth);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    spdlog::error("Deferred render framebuffer is not complete!");

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  m_Shader = CShaderLibrary::Load("res/shaders/deferred_pass_shader");

  if (!m_Shader.IsValid())
    spdlog::error("!!! ERROR: Failed to load deferred shadow shader !!!");

  BindShaderUniformBlocks(m_Shader);
}

void GLRenderDeferredPassSystem::OnUpdate(registry_t & Registry_, float Delta_)
{
  glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

  glEnable(GL_DEPTH_TEST);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  RenderMeshesWithShader(
      Registry_,
      Registry_.view<TGLSolidMeshComponent, TGLShaderComponent, TTransformComponent>(entt::exclude<TInvisibleMeshTag>).each(),
      m_Shader
    );

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GLRenderDeferredPassSystem::OnDestroy(registry_t & Registry_)
{
  // Empty
}
