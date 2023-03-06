#include "DirectedLightMeshSystem.h"

#include <glm/common.hpp>

#include "game/components/lightning/DirectedLightComponent.h"
#include <glm/ext/matrix_transform.hpp>

#include "core/components/TransformComponent.h"
#include "game/components/render/GLUnbakedMeshComponent.h"
#include "game/components/tags/InvisibleMeshTag.h"
#include "game/factory/BlockFactory.h"
#include "game/resources/ShaderLibrary.h"
#include "game/utils/NumericUtils.h"

//
// ISystem
//

void CDirectedLightMeshSystem::OnCreate(registry_t & Registry_)
{
  auto [Entity, Mesh] = Create<TGLUnbakedSolidMeshComponent>(Registry_);

  Mesh = CBlockFactory::CreateRawCubeMesh();

  AddComponent<TGLShaderComponent>(Registry_, Entity, CShaderLibrary::Load("res/shaders/directed_light_shader"));
  AddComponent<TTransformComponent>(Registry_, Entity, TTransformComponent
  {
    .Transform = glm::mat4(1.0)
  });

  AddTag<TInvisibleMeshTag>(Registry_, Entity);

  m_MeshEntity = Entity;
}

void CDirectedLightMeshSystem::OnUpdate(registry_t & Registry_, float Delta_)
{
  auto & Transform              = GetComponent<TTransformComponent>(Registry_, m_MeshEntity);
  auto & DirectedLightComponent = QuerySingle<TDirectedLightComponent>(Registry_);

  Transform.Transform = glm::translate(glm::mat4(1.0), DirectedLightComponent.Direction);
}

void CDirectedLightMeshSystem::OnDestroy(registry_t & Registry_)
{
  // Empty
}

