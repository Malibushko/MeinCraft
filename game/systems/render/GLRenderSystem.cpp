#include "GLRenderSystem.h"

#include <glad/glad.h>
#include <spdlog/spdlog.h>

#include "game/components/display/DisplayComponent.h"
#include "game/components/render/GLMeshComponent.h"
#include "game/components/render/GLShaderComponent.h"
#include "game/components/render/GLTextureComponent.h"

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

  const auto & Display = QueryFirst<TDisplayComponent>(Registry_);

  glViewport(0, 0, static_cast<int>(Display.Width), static_cast<int>(Display.Height));
  glEnable(GL_DEPTH_TEST);
}

void GLRenderSystem::OnUpdate(registry_t & Registry_, float Delta_)
{
  glClearColor(0.52f, 0.807f, 0.92f, 1.f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  for (const auto & [Entity, Mesh] : Registry_.view<TGLMeshComponent>().each())
  {
    assert(Mesh.IsBaked());

    if (const TGLShaderComponent * Shader = Registry_.try_get<TGLShaderComponent>(Entity); Shader)
    {
      assert(Shader->IsValid());
      glUseProgram(Shader->ShaderID);
    }

    if (const TGLTextureComponent * Texture = Registry_.try_get<TGLTextureComponent>(Entity); Texture)
    {
      assert(Texture->IsValid());

      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, Texture->TextureID);
    }

    glBindVertexArray(Mesh.VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Mesh.EBO);

    glDrawElements(GL_TRIANGLES, Mesh.IndicesCount, GL_UNSIGNED_INT, nullptr);
  }
}

void GLRenderSystem::OnDestroy(registry_t & Registry_)
{
  // Empty
}
