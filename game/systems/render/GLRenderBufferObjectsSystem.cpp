#include "GLRenderBufferObjectsSystem.h"

#include <glad/glad.h>
#include <spdlog/spdlog.h>
#include <glm/gtc/matrix_transform.hpp>
#include <magic_enum.hpp>

#include "game/components/camera/CameraBasisComponent.h"
#include "game/components/display/DisplayComponent.h"
#include "game/components/events/CameraChangedEvent.h"
#include "game/components/lightning/DirectedLightComponent.h"
#include "game/components/lightning/LightComponent.h"
#include "game/components/lightning/PointLightComponent.h"
#include "game/components/render/GLRenderPassData.h"
#include "game/components/render/GLShaderComponent.h"
#include "game/components/render/MaterialComponent.h"
#include "game/factory/BlockFactory.h"
#include "game/utils/GLRenderUtils.h"

static constexpr int MAX_LIGHTS_COUNT = 1024;
static constexpr int POINT_LIGHT_TILE_SIZE = 16;

//
// ISystem
//

void GLRenderBufferObjectsSystem::OnCreate(registry_t & Registry_)
{
  UpdateStorageBuffers(Registry_);

  UpdateMaterialsBuffer(Registry_);
}

void GLRenderBufferObjectsSystem::OnUpdate(registry_t & Registry_, float Delta_)
{
  if (HasComponent<TCameraChangedEvent>(Registry_))
    UpdateStorageBuffers(Registry_);
  else
    UpdateDirectedLightBuffer(Registry_);
}

void GLRenderBufferObjectsSystem::OnDestroy(registry_t & Registry_)
{
  // Empty
}

void GLRenderBufferObjectsSystem::UpdateStorageBuffers(registry_t & Registry_)
{
  UpdateMatricesBuffer(Registry_);
  UpdateCameraBuffer(Registry_);
  UpdateDirectedLightBuffer(Registry_);
  UpdatePointLightsBuffer(Registry_);
}

void GLRenderBufferObjectsSystem::UpdateMatricesBuffer(registry_t & Registry_)
{
  struct TMatricesUBO
  {
    alignas(sizeof(float) * 4) glm::mat4 Projection;
    alignas(sizeof(float) * 4) glm::mat4 View;
    alignas(sizeof(float) * 4) glm::mat4 MVP;
  } UBO;

  static_assert(std::is_standard_layout_v<TMatricesUBO>);

  auto & Transform  = QuerySingle<TGlobalTransformComponent>(Registry_);
  auto & RenderData = QuerySingle<TGLRenderPassData>(Registry_);

  UBO.Projection = Transform.Projection;
  UBO.View       = Transform.View;
  UBO.MVP        = Transform.Projection * Transform.View * Transform.Model;

  if (RenderData.MatricesBuffer == 0)
  {
    glGenBuffers(1, &RenderData.MatricesBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, RenderData.MatricesBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(TMatricesUBO), &UBO, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, static_cast<GLuint>(EShaderBuffer::MatricesBuffer), RenderData.MatricesBuffer);
  }
  else
  {
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, RenderData.MatricesBuffer);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(TMatricesUBO), &UBO);
  }

  glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void GLRenderBufferObjectsSystem::UpdateCameraBuffer(registry_t & Registry_)
{
  struct TCameraUBO
  {
    float                                ViewDistance;
    alignas(sizeof(float) * 4) glm::vec3 Direction;
    alignas(sizeof(float) * 4) glm::vec3 Position;
  } UBO;

  static_assert(std::is_standard_layout_v<TCameraUBO>);

  const auto & [CameraBasis, Position, Transform] = QuerySingle<TCameraBasisComponent, TPositionComponent, TGlobalTransformComponent>(Registry_);
  auto & RenderData = QuerySingle<TGLRenderPassData>(Registry_);

  UBO.Direction    = CameraBasis.Front;
  UBO.Position     = Position.Position;
  UBO.ViewDistance = static_cast<float>(RenderData.RenderFrustum.GetViewDistance());

  if (RenderData.CameraBuffer == 0)
  {
    glGenBuffers(1, &RenderData.CameraBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, RenderData.CameraBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(TCameraUBO), &UBO, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, static_cast<GLuint>(EShaderBuffer::CameraBuffer), RenderData.CameraBuffer);
  }
  else
  {
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, RenderData.CameraBuffer);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(TCameraUBO), &UBO);
  }

  glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void GLRenderBufferObjectsSystem::UpdateDirectedLightBuffer(registry_t & Registry_)
{
#pragma pack (push, 1)

  struct TLightUBO
  {
    float                                DirectedLightIntensity;
    alignas(sizeof(float) * 4) glm::vec3 DirectedLightDirection;
    alignas(sizeof(float) * 4) glm::vec3 DirectedLightColor;
    alignas(sizeof(float) * 4) glm::mat4 DirectedLightSpaceMatrix;
  } UBO;

#pragma pack (pop, 1)

  static_assert(std::is_standard_layout_v<TLightUBO>);
  static_assert(sizeof(TLightUBO) % 16 == 0);

  auto &&      [DirectedLight, Light]             = QuerySingle<TDirectedLightComponent, TLightComponent>(Registry_);
  const auto & [CameraBasis, Position, Transform] = QuerySingle<TCameraBasisComponent, TPositionComponent, TGlobalTransformComponent>(Registry_);
  auto &       RenderData                         = QuerySingle<TGLRenderPassData>(Registry_);

  const glm::mat4 LightProjection = glm::ortho(-100.f, 100.f, -100.f, 100.f, 0.1f, 250.f);
  const glm::mat4 LightView       = glm::lookAt(DirectedLight.Direction, glm::vec3(0.f), glm::vec3(0.f, 1.f, 0.f));

  UBO.DirectedLightDirection   = DirectedLight.Direction;
  UBO.DirectedLightIntensity   = DirectedLight.Intensity;
  UBO.DirectedLightColor       = Light.Color;
  UBO.DirectedLightSpaceMatrix = LightProjection * LightView;

  if (RenderData.DirectedLightBuffer == 0)
  {
    glGenBuffers(1, &RenderData.DirectedLightBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, RenderData.DirectedLightBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(TLightUBO), &UBO, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, static_cast<GLuint>(EShaderBuffer::DirectedLightBuffer), RenderData.DirectedLightBuffer);
  }
  else
  {
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, RenderData.DirectedLightBuffer);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(TLightUBO), &UBO);
  }

  glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void GLRenderBufferObjectsSystem::UpdatePointLightsBuffer(registry_t & Registry)
{
  auto & RenderData = QuerySingle<TGLRenderPassData>(Registry);
  auto & Display    = QuerySingle<TDisplayComponent>(Registry);

  struct TPointLight
  {
    glm::vec4 Position;
    glm::vec4 Color;

    float Constant{};
    float Linear{};
    float Quadratic{};
    float Radius{};
  };

  RenderData.LightCullingWorkGroupsX = (Display.Width  + (Display.Width % POINT_LIGHT_TILE_SIZE)) / POINT_LIGHT_TILE_SIZE;
  RenderData.LightCullingWorkGroupsY = (Display.Height + (Display.Height % POINT_LIGHT_TILE_SIZE)) / POINT_LIGHT_TILE_SIZE;

  const size_t TilesCount = RenderData.LightCullingWorkGroupsX * RenderData.LightCullingWorkGroupsY;

  if (RenderData.PointLightsBuffer == 0)
  {
    glGenBuffers(1, &RenderData.PointLightsBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, RenderData.PointLightsBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(TPointLight) * MAX_LIGHTS_COUNT, nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, static_cast<GLuint>(EShaderBuffer::PointLightsBuffer), RenderData.PointLightsBuffer);
  }

  if (RenderData.PointLightIndicesBuffer == 0)
  {
    glGenBuffers(1, &RenderData.PointLightIndicesBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, RenderData.PointLightIndicesBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(int) * TilesCount * MAX_LIGHTS_COUNT, nullptr, GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, static_cast<GLuint>(EShaderBuffer::VisiblePointLightsIndicesBuffer), RenderData.PointLightIndicesBuffer);
  }

  glBindBuffer(GL_SHADER_STORAGE_BUFFER, RenderData.PointLightsBuffer);

  TPointLight * Lights = static_cast<TPointLight *>(glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_WRITE));

  int LightsCount = 0;

  // TODO: somehow sort lights by distance to view frustum to fit the limit of MAX_LIGHTS_COUNT
  for (auto [Entity, PointLight, Light, Position] : Registry.view<TPointLightComponent, TLightComponent, TPositionComponent>().each())
  {
    Lights[LightsCount].Position  = glm::vec4(Position.Position, 1.0f);
    Lights[LightsCount].Color     = Light.Color;
    Lights[LightsCount].Constant  = PointLight.FadeConstant;
    Lights[LightsCount].Linear    = PointLight.FadeLinear;
    Lights[LightsCount].Quadratic = PointLight.FadeQuadratic;
    Lights[LightsCount].Radius    = PointLight.Radius;

    if (++LightsCount >= MAX_LIGHTS_COUNT)
      break;
  }

  glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void GLRenderBufferObjectsSystem::UpdateMaterialsBuffer(registry_t & Registry)
{
  struct TMaterial
  {
    float                                Metallic;
    float                                Roughness;
    float                                Emissive;
    alignas(sizeof(float) * 4) glm::vec4 EmissiveColor;
  };

  auto & RenderData = QuerySingle<TGLRenderPassData>(Registry);
  auto & Materials  = CBlockFactory::GetBlockMaterials();

  if (RenderData.MaterialsBuffer == 0)
  {
    glGenBuffers(1, &RenderData.MaterialsBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, RenderData.MaterialsBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(TMaterial) * Materials.size(), nullptr, GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, static_cast<GLuint>(EShaderBuffer::MaterialsBuffer), RenderData.MaterialsBuffer);
  }

  glBindBuffer(GL_SHADER_STORAGE_BUFFER, RenderData.MaterialsBuffer);

  TMaterial * MaterialsData = static_cast<TMaterial *>(glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_WRITE));

  int MaterialIndex = 0;

  for (const auto & Material : Materials)
  {
    MaterialsData[MaterialIndex].Metallic      = Material.Metallic;
    MaterialsData[MaterialIndex].Roughness     = Material.Roughness;
    MaterialsData[MaterialIndex].Emissive      = Material.Emissive;
    MaterialsData[MaterialIndex].EmissiveColor = Material.EmissiveColor;

    MaterialIndex++;
  }

  glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}
