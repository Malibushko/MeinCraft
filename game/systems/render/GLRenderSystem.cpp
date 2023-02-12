#include "GLRenderSystem.h"

#include <glad/glad.h>
#include <spdlog/spdlog.h>
#include <glm/gtc/matrix_transform.hpp>
#include <magic_enum.hpp>
#include "core/components/PositionComponent.h"
#include "core/components/TransformComponent.h"
#include "game/components/camera/CameraBasisComponent.h"

#include "game/components/camera/GlobalTransformComponent.h"
#include "game/components/camera/PerspectiveCameraComponent.h"
#include "game/components/display/DisplayComponent.h"
#include "game/components/events/CameraChangedEvent.h"
#include "game/components/physics/BoundingVolume.h"
#include "game/components/render/GLMeshComponent.h"
#include "game/components/render/GLShaderComponent.h"
#include "game/components/render/GLTextureComponent.h"

enum class EUniformBlock
{
  MatricesBlock,
  CameraBlock,
  LightBlock
};


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
  glEnable(GL_DEPTH_TEST);
}

void GLRenderSystem::OnUpdate(registry_t & Registry_, float Delta_)
{
  glClearColor(0.52f, 0.807f, 0.92f, 1.f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  const auto & GlobalTransform = QueryOrCreate<TGlobalTransformComponent>(Registry_).second;
  const auto & CameraBasis     = QuerySingle<TCameraBasisComponent>(Registry_);

  if (IsNeedUpdateFrustum(Registry_))
  {
    UpdateFrustum(GlobalTransform);
    UpdateUniformBlocks(Registry_);
  }

  GLuint PreviousShader  = 0;
  GLuint PreviousTexture = 0;

  for (auto && [Entity, Mesh, Shader, Texture, Transform] : Registry_.view<TGLMeshComponent, TGLShaderComponent, TGLTextureComponent, TTransformComponent>().each())
  {
    if (const TBoundingVolumeComponent * BBComponent = Registry_.try_get<TBoundingVolumeComponent>(Entity))
    {
      if (!m_RenderFrustum.Intersect(*BBComponent))
        continue;
    }

    assert(Mesh.IsBaked());

    if (PreviousShader != Shader.ShaderID)
    {
      assert(Shader.IsValid());
      glUseProgram(Shader.ShaderID);

      PreviousShader = Shader.ShaderID;

      magic_enum::enum_for_each<EUniformBlock>([&](EUniformBlock UniformBlock)
      {
        const auto & BlockName = magic_enum::enum_name(UniformBlock);
        const auto   BlockID   = glGetUniformBlockIndex(Shader.ShaderID, BlockName.data());

        glUniformBlockBinding(Shader.ShaderID, BlockID, static_cast<GLuint>(UniformBlock));
      });
    }

    glUniformMatrix4fv(
        glGetUniformLocation(Shader.ShaderID, "u_Transform"),
        1,
        GL_FALSE,
        &Transform.Transform[0][0]
      );

    assert(Texture.IsValid());

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

void GLRenderSystem::UpdateFrustum(const TGlobalTransformComponent & Transform)
{
  m_RenderFrustum       = CFrustum::FromTransform(Transform);
  m_IsNeedUpdateFrustum = false;
}

void GLRenderSystem::UpdateUniformBlocks(registry_t & Registry_)
{
  UpdateMatricesUBO(Registry_);
  UpdateCameraUBO(Registry_);
  UpdateLightUBO(Registry_);
}

bool GLRenderSystem::IsNeedUpdateFrustum(registry_t & Registry) const
{
  if (m_IsNeedUpdateFrustum)
    return true;

  return HasComponent<TCameraChangedEvent>(Registry);
}

void GLRenderSystem::UpdateMatricesUBO(registry_t & Registry_)
{
  struct TMatricesUBO
  {
    glm::mat4 Projection;
    glm::mat4 View;
    glm::mat4 MVP;
  } UBO;

  static_assert(std::is_standard_layout_v<TMatricesUBO>);

  const auto & Transform = QuerySingle<TGlobalTransformComponent>(Registry_);

  UBO.Projection = Transform.Projection;
  UBO.View       = Transform.View;
  UBO.MVP        = Transform.Projection * Transform.View * Transform.Model;

  if (m_MatricesUBO == 0)
    glGenBuffers(1, &m_MatricesUBO);

  glBindBuffer(GL_UNIFORM_BUFFER, m_MatricesUBO);

  glBufferData(GL_UNIFORM_BUFFER, sizeof(TMatricesUBO), nullptr, GL_DYNAMIC_DRAW);
  glBindBufferRange(GL_UNIFORM_BUFFER, static_cast<GLuint>(EUniformBlock::MatricesBlock), m_MatricesUBO, 0, sizeof(TMatricesUBO));
  glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(TMatricesUBO), &UBO);

  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void GLRenderSystem::UpdateCameraUBO(registry_t & Registry_)
{
  struct TCameraUBO
  {
    glm::vec3 Direction;
    glm::vec3 Position;
    float     ViewDistance;
  } UBO;

  static_assert(std::is_standard_layout_v<TCameraUBO>);

  const auto & CameraBasis = QuerySingle<TCameraBasisComponent>(Registry_);
  const auto & Transform   = QuerySingle<TGlobalTransformComponent>(Registry_);

  UBO.Direction    = CameraBasis.Front;
  UBO.Position     = Transform.View[3];
  UBO.ViewDistance = m_RenderFrustum.GetViewDistance();

  if (m_CameraUBO == 0)
    glGenBuffers(1, &m_CameraUBO);

  glBindBuffer(GL_UNIFORM_BUFFER, m_CameraUBO);

  glBufferData(GL_UNIFORM_BUFFER, sizeof(TCameraUBO), nullptr, GL_DYNAMIC_DRAW);
  glBindBufferRange(GL_UNIFORM_BUFFER, static_cast<GLuint>(EUniformBlock::CameraBlock), m_CameraUBO, 0, sizeof(TCameraUBO));
  glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(m_CameraUBO), &UBO);

  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void GLRenderSystem::UpdateLightUBO(registry_t & Registry_)
{
}
