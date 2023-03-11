#include "GLRenderUniformObjectsSystem.h"

#include <glad/glad.h>
#include <spdlog/spdlog.h>
#include <glm/gtc/matrix_transform.hpp>
#include <magic_enum.hpp>

#include "game/components/camera/CameraBasisComponent.h"
#include "game/components/events/CameraChangedEvent.h"
#include "game/components/lightning/DirectedLightComponent.h"
#include "game/components/lightning/LightComponent.h"
#include "game/components/lightning/PointLightComponent.h"
#include "game/components/render/GLRenderPassData.h"
#include "game/components/render/GLShaderComponent.h"
#include "game/utils/GLRenderUtils.h"

static constexpr int MAX_LIGHTS = 64;

//
// ISystem
//

void GLRenderUniformObjectsSystem::OnCreate(registry_t & Registry_)
{
  Registry_.on_construct<TGLShaderComponent>().connect<&GLRenderUniformObjectsSystem::OnShaderCreated>(this);
}

void GLRenderUniformObjectsSystem::OnUpdate(registry_t & Registry_, float Delta_)
{
  if (HasComponent<TCameraChangedEvent>(Registry_))
    UpdateUniformBlocks(Registry_);
  else
    UpdateLightUBO(Registry_);
}

void GLRenderUniformObjectsSystem::OnDestroy(registry_t & Registry_)
{
  // Empty
}

void GLRenderUniformObjectsSystem::UpdateUniformBlocks(registry_t & Registry_)
{
  UpdateMatricesUBO(Registry_);
  UpdateCameraUBO(Registry_);
  UpdateLightUBO(Registry_);
}

void GLRenderUniformObjectsSystem::UpdateMatricesUBO(registry_t & Registry_)
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

  if (RenderData.MatricesUBO == 0)
    glGenBuffers(1, &RenderData.MatricesUBO);

  glBindBuffer(GL_UNIFORM_BUFFER, RenderData.MatricesUBO);

  glBufferData(GL_UNIFORM_BUFFER, sizeof(TMatricesUBO), nullptr, GL_DYNAMIC_DRAW);
  glBindBufferRange(GL_UNIFORM_BUFFER, static_cast<GLuint>(EUniformBlock::MatricesBlock), RenderData.MatricesUBO, 0, sizeof(TMatricesUBO));
  glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(TMatricesUBO), &UBO);

  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void GLRenderUniformObjectsSystem::UpdateCameraUBO(registry_t & Registry_)
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

  if (RenderData.CameraUBO == 0)
    glGenBuffers(1, &RenderData.CameraUBO);

  glBindBuffer(GL_UNIFORM_BUFFER, RenderData.CameraUBO);

  glBufferData(GL_UNIFORM_BUFFER, sizeof(TCameraUBO), nullptr, GL_DYNAMIC_DRAW);
  glBindBufferRange(GL_UNIFORM_BUFFER, static_cast<GLuint>(EUniformBlock::CameraBlock), RenderData.CameraUBO, 0, sizeof(TCameraUBO));
  glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(TCameraUBO), &UBO);

  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void GLRenderUniformObjectsSystem::UpdateLightUBO(registry_t & Registry_)
{
#pragma pack (push, 1)

  struct TPointLight
  {
    alignas(sizeof(float) * 4) glm::vec3 Position;
    alignas(sizeof(float) * 4) glm::vec3 Color;

    float Constant{};
    float Linear{};
    float Quadratic{};
    float Radius{};
  };

  struct TLightUBO
  {
    float                                DirectedLightIntensity;
    alignas(sizeof(float) * 4) glm::vec3 DirectedLightDirection;
    alignas(sizeof(float) * 4) glm::vec3 DirectedLightColor;
    alignas(sizeof(float) * 4) glm::mat4 DirectedLightSpaceMatrix;

    TPointLight                          PointLights[MAX_LIGHTS];
  } UBO;

#pragma pack (pop, 1)

  static_assert(std::is_standard_layout_v<TLightUBO>);

  auto && [DirectedLight, Light] = QuerySingle<TDirectedLightComponent, TLightComponent>(Registry_);
  auto & RenderData              = QuerySingle<TGLRenderPassData>(Registry_);

  const glm::mat4 LightProjection = glm::ortho(-100.f, 100.f, -100.f, 100.f, 0.1f, 250.f);
  const glm::mat4 LightView       = glm::lookAt(DirectedLight.Direction, glm::vec3(0.f), glm::vec3(0.f, 1.f, 0.f));

  UBO.DirectedLightDirection   = DirectedLight.Direction;
  UBO.DirectedLightIntensity   = DirectedLight.Intensity;
  UBO.DirectedLightColor       = Light.Color;
  UBO.DirectedLightSpaceMatrix = LightProjection * LightView;

  int LightsCount = 0;

  // TODO: somehow sort lights by distance to view frustrum to fit the limit of 64
  for (auto [Entity, PointLight, Light, Position] : Registry_.view<TPointLightComponent, TLightComponent, TPositionComponent>().each())
  {
    UBO.PointLights[LightsCount].Position  = Position.Position;
    UBO.PointLights[LightsCount].Color     = Light.Color;
    UBO.PointLights[LightsCount].Constant  = PointLight.FadeConstant;
    UBO.PointLights[LightsCount].Linear    = PointLight.FadeLinear;
    UBO.PointLights[LightsCount].Quadratic = PointLight.FadeQuadratic;
    UBO.PointLights[LightsCount].Radius    = PointLight.Radius;

    if (++LightsCount >= MAX_LIGHTS)
      break;
  }

  if (RenderData.LightUBO == 0)
    glGenBuffers(1, &RenderData.LightUBO);

  glBindBuffer(GL_UNIFORM_BUFFER, RenderData.LightUBO);

  glBufferData(GL_UNIFORM_BUFFER, sizeof(TLightUBO), nullptr, GL_DYNAMIC_DRAW);
  glBindBufferRange(GL_UNIFORM_BUFFER, static_cast<GLuint>(EUniformBlock::LightBlock), RenderData.LightUBO, 0, sizeof(TLightUBO));
  glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(TLightUBO), &UBO);

  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

//
// Events
//

void GLRenderUniformObjectsSystem::OnShaderCreated(registry_t & Registry, entity_t Entity)
{
  TGLShaderComponent & ShaderComponent = Registry.get<TGLShaderComponent>(Entity);

  if (ShaderComponent.IsValid())
    BindShaderUniformBlocks(ShaderComponent);
  else
    spdlog::warn("GLRenderUniformObjectsSystem::OnShaderCreated: Shader is not valid");
}
