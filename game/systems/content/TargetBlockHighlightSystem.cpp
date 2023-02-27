#include "TargetBlockHighlightSystem.h"

#include <glm/ext/matrix_transform.hpp>

#include "core/components/PositionComponent.h"
#include "core/components/TransformComponent.h"
#include "game/components/camera/CameraBasisComponent.h"
#include "game/components/camera/GlobalTransformComponent.h"
#include "game/components/content/CameraTargetComponent.h"
#include "game/components/events/CameraChangedEvent.h"
#include "game/components/requests/RaycastRequest.h"
#include "game/components/terrain/BlockComponent.h"
#include "game/factory/BlockFactory.h"
#include "game/resources/ShaderLibrary.h"
#include "game/resources/TextureLibrary.h"

const std::vector<glm::vec2> BLOCK_HIGHLIGH_UV
{
  // Front
  glm::vec2(0.0, 0.0),
  glm::vec2(0.0, 1.0),
  glm::vec2(1.0, 1.0),
  glm::vec2(1.0, 0.0),
  // Back
  glm::vec2(0.0, 0.0),
  glm::vec2(0.0, 1.0),
  glm::vec2(1.0, 1.0),
  glm::vec2(1.0, 0.0),
  // Left
  glm::vec2(0.0, 0.0),
  glm::vec2(0.0, 1.0),
  glm::vec2(1.0, 1.0),
  glm::vec2(1.0, 0.0),
  // Right
  glm::vec2(0.0, 0.0),
  glm::vec2(0.0, 1.0),
  glm::vec2(1.0, 1.0),
  glm::vec2(1.0, 0.0),
  // Top
  glm::vec2(0.0, 0.0),
  glm::vec2(0.0, 1.0),
  glm::vec2(1.0, 1.0),
  glm::vec2(1.0, 0.0),
  // Bottom
  glm::vec2(0.0, 0.0),
  glm::vec2(0.0, 1.0),
  glm::vec2(1.0, 1.0),
  glm::vec2(1.0, 0.0),
};

//
// Construction/Destruction
//

CTargetBlockHighlightSystem::CTargetBlockHighlightSystem() = default;

CTargetBlockHighlightSystem::~CTargetBlockHighlightSystem() = default;

//
// ISystem
//

void CTargetBlockHighlightSystem::OnCreate(registry_t& Registry_)
{
  // Empty
}

void CTargetBlockHighlightSystem::OnUpdate(registry_t & Registry_, float Delta_)
{
  auto & Target = QuerySingle<TCameraTargetComponent>(Registry_);

  if (Target.Refreshed)
    UpdateTarget(Registry_, Target);
}


void CTargetBlockHighlightSystem::OnDestroy(registry_t& Registry_)
{

}

void CTargetBlockHighlightSystem::UpdateTarget(registry_t & Registry, TCameraTargetComponent & Target)
{
  if (Target.Target == entt::null || !Registry.try_get<TBlockComponent>(Target.Target))
  {
    if (m_MeshEntity != entt::null)
      Registry.destroy(m_MeshEntity);

    m_MeshEntity = entt::null;

    return;
  }

  if (m_MeshEntity == entt::null)
  {
    m_MeshEntity = Registry.create();

    TGLUnbakedSolidMeshComponent Mesh = CBlockFactory::GetMeshForBlock(GetComponent<TBlockComponent>(Registry, Target.Target), EBlockFace::All);

    Mesh.UV = BLOCK_HIGHLIGH_UV;

    Registry.emplace_or_replace<TGLUnbakedSolidMeshComponent>(m_MeshEntity, std::move(Mesh));
    Registry.emplace_or_replace<TGLTextureComponent>(m_MeshEntity, TGLTextureComponent
    {
      .TextureID = CTextureLibrary::Load("res/textures/block_selection.png").TextureID
    });

    Registry.emplace_or_replace<TGLShaderComponent>(m_MeshEntity, CShaderLibrary::Load("res/shaders/blocks_outline_shader"));
  }

  TTransformComponent Transform
  {
    .Transform = glm::translate(glm::mat4(1.0), glm::floor(Target.WorldPosition))
  };

  Registry.emplace_or_replace<TTransformComponent>(m_MeshEntity, Transform);
}



