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
#include "game/components/lightning/DirectedLightComponent.h"
#include "game/components/lightning/LightComponent.h"
#include "game/components/physics/BoundingVolume.h"
#include "game/components/render/GLMeshComponent.h"
#include "game/components/render/GLShaderComponent.h"
#include "game/components/render/GLTextureComponent.h"
#include "game/components/render/GLUnbakedMeshComponent.h"

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

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void GLRenderSystem::OnUpdate(registry_t & Registry_, float Delta_)
{
  glClearColor(0.52f, 0.807f, 0.92f, 1.f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  if (IsNeedUpdateFrustum(Registry_))
  {
    UpdateFrustum(QueryOrCreate<TGlobalTransformComponent>(Registry_).second);
    UpdateUniformBlocks(Registry_);
    UpdateTransluscentMeshesOrder(Registry_);
  }

  UpdateLightUBO(Registry_);

  GLuint PreviousShader  = 0;
  GLuint PreviousTexture = 0;

  const auto RenderPass = [&](auto && Meshes)
  {
    for (auto && [Entity, Mesh, Shader, Texture, Transform] : Meshes.each())
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
          const auto   BlockID = glGetUniformBlockIndex(Shader.ShaderID, BlockName.data());

          assert(BlockID != GL_INVALID_INDEX);
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
      glDrawElements(GL_TRIANGLES, Mesh.IndicesCount, GL_UNSIGNED_SHORT, nullptr);
    }
  };

  auto && SolidMeshes = Registry_.view<TGLSolidMeshComponent, TGLShaderComponent, TGLTextureComponent, TTransformComponent>();
  auto && TranslucentMeshes = Registry_.view<TGLTranslucentMeshComponent, TGLShaderComponent, TGLTextureComponent, TTransformComponent>();

  RenderPass(SolidMeshes);
  RenderPass(TranslucentMeshes);
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
    float                                ViewDistance;
    alignas(sizeof(float) * 4) glm::vec3 Position;
    alignas(sizeof(float) * 4) glm::vec3 Direction;
  } UBO;

  static_assert(std::is_standard_layout_v<TCameraUBO>);

  const auto & [CameraBasis, Position] = QuerySingle<TCameraBasisComponent, TPositionComponent>(Registry_);
  const auto & Transform               = QuerySingle<TGlobalTransformComponent>(Registry_);

  UBO.Direction    = CameraBasis.Front;
  UBO.Position     = glm::vec3(0.f);
  UBO.ViewDistance = static_cast<float>(m_RenderFrustum.GetViewDistance());

  if (m_CameraUBO == 0)
    glGenBuffers(1, &m_CameraUBO);

  glBindBuffer(GL_UNIFORM_BUFFER, m_CameraUBO);

  glBufferData(GL_UNIFORM_BUFFER, sizeof(TCameraUBO), nullptr, GL_DYNAMIC_DRAW);
  glBindBufferRange(GL_UNIFORM_BUFFER, static_cast<GLuint>(EUniformBlock::CameraBlock), m_CameraUBO, 0, sizeof(TCameraUBO));
  glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(TCameraUBO), &UBO);

  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void GLRenderSystem::UpdateLightUBO(registry_t & Registry_)
{
  struct TLightUBO
  {
    float                                DirectedLightIntensity;
    alignas(sizeof(float) * 4) glm::vec3 DirectedLightDirection;
    alignas(sizeof(float) * 4) glm::vec3 DirectedLightColor;
  } UBO;

  static_assert(std::is_standard_layout_v<TLightUBO>);

  auto && [DirectedLight, Light] = QuerySingle<TDirectedLightComponent, TLightComponent>(Registry_);

  UBO.DirectedLightDirection = DirectedLight.Direction;
  UBO.DirectedLightIntensity = DirectedLight.Intensity;
  UBO.DirectedLightColor     = Light.Ambient + Light.Diffuse + Light.Specular;

  if (m_LightUBO == 0)
    glGenBuffers(1, &m_LightUBO);

  glBindBuffer(GL_UNIFORM_BUFFER, m_LightUBO);

  glBufferData(GL_UNIFORM_BUFFER, sizeof(TLightUBO), nullptr, GL_DYNAMIC_DRAW);
  glBindBufferRange(GL_UNIFORM_BUFFER, static_cast<GLuint>(EUniformBlock::LightBlock), m_LightUBO, 0, sizeof(TLightUBO));
  glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(TLightUBO), &UBO);

  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void GLRenderSystem::UpdateTransluscentMeshesOrder(registry_t & Registry_)
{
  auto & Transform = QueryOrCreate<TGlobalTransformComponent>(Registry_).second;

  glm::vec3 ViewPosition = m_RenderFrustum.GetPosition();

  Registry_.sort<TGLTranslucentMeshComponent>([&](const entity_t Left, const entity_t Right) -> bool
  {
    const glm::vec3 LeftPosition  = Registry_.get<TTransformComponent>(Left).Transform[3];
    const glm::vec3 RightPosition = Registry_.get<TTransformComponent>(Right).Transform[3];

    return glm::length(ViewPosition - LeftPosition) - glm::length(ViewPosition - RightPosition);
  });
}
