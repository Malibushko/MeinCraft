#include "GLRenderSystem.h"

#include <glad/glad.h>
#include <spdlog/spdlog.h>

#include "game/components/camera/GlobalTransformComponent.h"
#include "game/components/display/DisplayComponent.h"
#include "game/components/events/CameraChangedEvent.h"
#include "game/components/render/GLRenderPassData.h"

//
// Construction/Destruction
//

GLRenderSystem::GLRenderSystem() = default;

GLRenderSystem::~GLRenderSystem() = default;

//
// ISystem
//

void GLRenderSystem::OnCreate(registry_t & Registry_)
{
  if (gladLoadGL() == 0)
    spdlog::critical("Failed to initialize OpenGL context");

  const auto & Display = QuerySingle<TDisplayComponent>(Registry_);

  glViewport(0, 0, static_cast<int>(Display.Width), static_cast<int>(Display.Height));
  glEnable(GL_FRAMEBUFFER_SRGB);

#ifndef NDEBUG

  glEnable(GL_DEBUG_OUTPUT);

  glDebugMessageCallback([](GLenum, GLenum Type, GLuint, GLenum, GLsizei, const GLchar * Message, const void *)
  {
    if (Type == GL_DEBUG_TYPE_ERROR)
      spdlog::error("OpenGL error message: {})", Message);
  }, nullptr);

#endif

  Create<TGLRenderPassData>(Registry_);
}

void GLRenderSystem::OnUpdate(registry_t & Registry_, float Delta_)
{
  if (IsNeedUpdateFrustum(Registry_))
  {
    TGLRenderPassData & Data = QuerySingle<TGLRenderPassData>(Registry_);

    Data.RenderFrustum = CFrustum::FromTransform(QueryOrCreate<TGlobalTransformComponent>(Registry_));
  }
}

void GLRenderSystem::OnDestroy(registry_t & Registry_)
{
  // Empty
}

bool GLRenderSystem::IsNeedUpdateFrustum(registry_t & Registry) const
{
  return HasComponent<TCameraChangedEvent>(Registry);
}
