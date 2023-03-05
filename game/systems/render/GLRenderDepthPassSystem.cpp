#include "GLRenderDepthPassSystem.h"

#include <spdlog/spdlog.h>

#include "GLRenderTransparentPassSystem.h"
#include "core/components/TransformComponent.h"
#include "game/components/display/DisplayComponent.h"
#include "game/components/render/GLMeshComponent.h"
#include "game/components/render/GLRenderPassData.h"
#include "game/components/render/GLTextureComponent.h"
#include "game/resources/ShaderLibrary.h"
#include "game/utils/GLRenderUtils.h"

//
// ISystem
//

void GLRenderDepthPassSystem::OnCreate(registry_t & Registry_)
{
  auto & RenderData = QueryOrCreate<TGLRenderPassData>(Registry_);
  auto & Display    = QuerySingle<TDisplayComponent>(Registry_);

  glGenFramebuffers(1, &m_FBO);

  glGenTextures(1, &RenderData.DepthTexture);
  glBindTexture(GL_TEXTURE_2D, RenderData.DepthTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, static_cast<GLsizei>(Display.Width), static_cast<GLsizei>(Display.Height), 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

  glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, RenderData.DepthTexture, 0);
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    spdlog::critical("!!! ERROR: Depth framebuffer is not complete !!!");

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  m_Shader = CShaderLibrary::Load("res/shaders/depth_shader");

  if (!m_Shader.IsValid())
    spdlog::error("!!! ERROR: Failed to load directional shadow shader !!!");
}

void GLRenderDepthPassSystem::OnUpdate(registry_t & Registry_, float Delta_)
{
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);

  glDepthFunc(GL_LESS);
  glDepthMask(GL_TRUE);

  glEnable(GL_CULL_FACE);
  glCullFace(GL_FRONT);

  glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
  glClear(GL_DEPTH_BUFFER_BIT);
  glUseProgram(m_Shader.ShaderID);

  RenderMeshesWithShader(
      Registry_,
      Registry_.view<TGLSolidMeshComponent, TGLShaderComponent, TTransformComponent>().each(),
      m_Shader
    );

  glDisable(GL_CULL_FACE);
}

void GLRenderDepthPassSystem::OnDestroy(registry_t & Registry_)
{
  // Empty
}
