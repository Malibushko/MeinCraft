#include "PhysicsSystem.h"

#include <glad/glad.h>

#include "core/components/TransformComponent.h"
#include "game/components/camera/CameraBasisComponent.h"
#include "game/components/display/DisplayComponent.h"
#include "game/components/requests/RaycastRequest.h"
#include "game/components/terrain/TerrainComponent.h"

//
// Construction/Destruction
//

CPhysicsSystem::CPhysicsSystem() = default;

CPhysicsSystem::~CPhysicsSystem() = default;

//
// Interface
//

void CPhysicsSystem::OnCreate(registry_t & Registry_)
{
  // Empty
}

void CPhysicsSystem::OnUpdate(registry_t & Registry_, float Delta_)
{
  ProcessRaycastRequests(Registry_);
}

void CPhysicsSystem::OnDestroy(registry_t & Registry_)
{
  // Empty
}

//
// Service
//

void CPhysicsSystem::ProcessRaycastRequests(registry_t& Registry)
{
  for (const auto & [Entity, Request] :Registry.view<TRaycastRequest>().each())
  {
    if (Request.Status != ERequestStatus::Created)
      continue;

    if (!Request.StartPosition.has_value())
      Request.StartPosition = CalculateDefaultRayPosition(Registry);

    if (!Request.Direction.has_value())
      Request.Direction = CalculateDefaultRayDirection(Registry);

    auto [HitEntity, EndPosition] = Raycast(Registry, *Request.StartPosition, *Request.Direction, Request.Distance);

    Request.RaycastHit  = HitEntity;
    Request.EndPosition = EndPosition;
    Request.Status      = ERequestStatus::Fulfilled;
  }
}

std::pair<entity_t, glm::vec3> CPhysicsSystem::Raycast(registry_t & Registry, glm::vec3 StartPosition, glm::vec3 Direction, float Distance) const
{
  glm::vec3 Step = Direction;

  glm::vec3 Position = StartPosition;

  float CurrentDistance = 0;

  while ((CurrentDistance = glm::distance(Position, StartPosition)) < Distance)
  {
    if (const auto Entity = GetEntityAt(Registry, Position); Entity != entt::null)
      return { Entity, Position };

    Position += Step;
  }

  return { entt::null, Position };
}

entity_t CPhysicsSystem::GetEntityAt(registry_t & Registry, glm::vec3 Position) const
{
  // NOTE: currently only checking for blocks

  const TTerrainComponent & Terrain     = QuerySingle<TTerrainComponent>(Registry);
  const entity_t            ChunkEntity = Terrain.GetChunkAt(Position);

  if (ChunkEntity == entt::null)
    return entt::null;

  const TChunkComponent & Chunk = GetComponent<TChunkComponent>(Registry, ChunkEntity);

  return Chunk.GetBlockAt(WorldToChunkPosition(Position));
}

glm::vec3 CPhysicsSystem::CalculateDefaultRayPosition(registry_t & Registry) const
{
  const auto & [Transform, Position] = QuerySingle<TGlobalTransformComponent, TPositionComponent>(Registry);

  return Position.Position;
}

glm::vec3 CPhysicsSystem::CalculateDefaultRayDirection(registry_t & Registry) const
{
  const auto & Display            = QuerySingle<TDisplayComponent>(Registry);
  const auto & [Transform, Basis] = QuerySingle<TGlobalTransformComponent, TCameraBasisComponent>(Registry);

  return Basis.Front;
}



