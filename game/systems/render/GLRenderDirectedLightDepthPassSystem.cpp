#include "GLRenderDirectedLightDepthPassSystem.h"

#include <spdlog/spdlog.h>

#include "GLRenderTransparentPassSystem.h"
#include "core/components/TransformComponent.h"
#include "game/components/display/DisplayComponent.h"
#include "game/components/render/GLMeshComponent.h"
#include "game/components/render/GLRenderPassData.h"
#include "game/components/render/GLTextureComponent.h"
#include "game/components/tags/InvisibleMeshTag.h"
#include "game/resources/ShaderLibrary.h"
#include "game/utils/GLRenderUtils.h"

static constexpr size_t SHADOW_MAP_WIDTH  = 4096;
static constexpr size_t SHADOW_MAP_HEIGHT = 4096;

//
// ISystem
//

void GLRenderDirectedLightDepthPassSystem::OnCreate(registry_t & Registry_)
{
  auto & RenderData = QueryOrCreate<TGLRenderPassData>(Registry_);
  auto & Display    = QuerySingle<TDisplayComponent>(Registry_);

  const float BorderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };

  glGenFramebuffers(1, &m_FBO);

  glGenTextures(1, &RenderData.DepthTexture);
  glBindTexture(GL_TEXTURE_2D, RenderData.DepthTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT,
                                                       0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

  glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, RenderData.DepthTexture, 0);
  glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, BorderColor);
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    spdlog::critical("!!! ERROR: Depth framebuffer is not complete !!!");

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  m_Shader = CShaderLibrary::Load("res/shaders/depth_shader");

  if (!m_Shader.IsValid())
    spdlog::error("!!! ERROR: Failed to load directional shadow shader !!!");

  BindShaderUniformBlocks(m_Shader);
}

void GLRenderDirectedLightDepthPassSystem::OnUpdate(registry_t & Registry_, float Delta_)
{
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glDepthMask(GL_TRUE);

  glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
  glClear(GL_DEPTH_BUFFER_BIT);

  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  const auto & Display = QuerySingle<TDisplayComponent>(Registry_);

  glViewport(0, 0, SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT);
  glEnable(GL_POLYGON_OFFSET_FILL);
  glPolygonOffset(1.1f, 4.0f); // hack to avoid shadow acne

  RenderMeshesWithShader(
      Registry_,
      Registry_.view<TGLSolidMeshComponent, TGLShaderComponent, TTransformComponent>(entt::exclude<TInvisibleMeshTag>).each(),
      m_Shader
    );

  glDisable(GL_POLYGON_OFFSET_FILL);
  glViewport(0, 0, Display.Width, Display.Height);
}

void GLRenderDirectedLightDepthPassSystem::OnDestroy(registry_t & Registry_)
{
  // Empty
}
