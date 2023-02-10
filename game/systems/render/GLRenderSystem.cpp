#include "GLRenderSystem.h"

#include <glad/glad.h>
#include <spdlog/spdlog.h>
#include <glm/gtc/matrix_transform.hpp>
#include "core/components/PositionComponent.h"
#include "core/components/TransformComponent.h"

#include "game/components/camera/GlobalTransformComponent.h"
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

  glm::mat4 GlobalTransform = QueryOrCreate<TGlobalTransformComponent>(Registry_).second.Transform;

  GLuint PreviousShader  = 0;
  GLuint PreviousTexture = 0;

  for (auto && [Entity, Mesh, Shader, Texture, Transform] : Registry_.view<TGLMeshComponent, TGLShaderComponent, TGLTextureComponent, TTransformComponent>().each())
  {
    assert(Mesh.IsBaked());

    if (PreviousShader != Shader.ShaderID)
    {
      assert(Shader.IsValid());
      glUseProgram(Shader.ShaderID);

      PreviousShader = Shader.ShaderID;
    }

    glUniformMatrix4fv(
        glGetUniformLocation(Shader.ShaderID, "u_Transform"),
        1,
        GL_FALSE,
        &(GlobalTransform * Transform.Transform)[0][0]
      );

    assert(Texture->IsValid());

    if (PreviousTexture != Texture.TextureID)
    {
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, Texture.TextureID);

      PreviousTexture = Texture.TextureID;
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
