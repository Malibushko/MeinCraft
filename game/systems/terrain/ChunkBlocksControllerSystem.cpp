#include "ChunkBlocksControllerSystem.h"

#include "core/components/PositionComponent.h"
#include "game/components/requests/BlockCreateRequest.h"
#include "game/components/requests/PlayerHitRequest.h"
#include "game/components/terrain/BlockComponent.h"
#include "game/components/terrain/ChunkComponent.h"
#include "game/components/terrain/TerrainComponent.h"

//
// Construction/Destruction
//

CChunkBlocksControllerSystem::CChunkBlocksControllerSystem() = default;

CChunkBlocksControllerSystem::~CChunkBlocksControllerSystem() = default;

//
// ISystem
//

void CChunkBlocksControllerSystem::OnCreate(registry_t & Registry_)
{
  // Empty
}

void CChunkBlocksControllerSystem::OnUpdate(registry_t & Registry_, float Delta_)
{
  ProcessRequests(Registry_);
}

void CChunkBlocksControllerSystem::OnDestroy(registry_t & Registry_)
{
  // Empty
}

void CChunkBlocksControllerSystem::ProcessRequests(registry_t & Registry)
{
  ProcessBlockHitRequests(Registry);
  ProcessBlockCreateRequests(Registry);
}

void CChunkBlocksControllerSystem::ProcessBlockCreateRequests(registry_t & Registry)
{
  for (auto [Entity, CreateRequest] : Registry.view<TBlockCreateRequest>().each())
  {
    if (CreateRequest.Status != ERequestStatus::Created)
      return;

    TTerrainComponent & Terrain            = QuerySingle<TTerrainComponent>(Registry);
    entity_t            ChunkEntity        = Terrain.GetChunkAt(CreateRequest.WorldPosition);

    if (ChunkEntity == entt::null)
    {
      CreateRequest.Status = ERequestStatus::Rejected;

      continue;
    }

    TChunkComponent &   Chunk              = Registry.get<TChunkComponent>(ChunkEntity);
    const glm::ivec3    BlockChunkPosition = WorldToChunkPosition(CreateRequest.WorldPosition);

    if (entity_t & BlockEntity = Chunk.GetBlockAt(BlockChunkPosition); BlockEntity != entt::null)
    {
      CreateRequest.Status = ERequestStatus::Rejected;
    }
    else
    {
      BlockEntity = Registry.create();

      AddComponent<TBlockComponent>(Registry, BlockEntity, TBlockComponent
      {
        .Type = CreateRequest.Type
      });

      Chunk.State = EChunkState::Dirty;

      CreateRequest.Status = ERequestStatus::Fulfilled;
    }
  }
}

void CChunkBlocksControllerSystem::ProcessBlockHitRequests(registry_t& Registry)
{
  for (auto [Entity, HitRequest] : Registry.view<TPlayerHitRequest>().each())
  {
    TBlockComponent * Block = Registry.try_get<TBlockComponent>(HitRequest.Target);

    if (!Block)
      continue;

    auto &            Terrain            = QuerySingle<TTerrainComponent>(Registry);
    entity_t          ChunkEntity        = Terrain.GetChunkAt(HitRequest.TargetPosition);
    TChunkComponent & Chunk              = Registry.get<TChunkComponent>(ChunkEntity);
    const glm::ivec3  BlockChunkPosition = WorldToChunkPosition(HitRequest.TargetPosition);

    Chunk.GetBlockAt(BlockChunkPosition) = entt::null;
    Registry.destroy(HitRequest.Target);

    Chunk.State = EChunkState::Dirty;

    const glm::ivec2 TargetChunkPosition = ToChunkCoordinates(HitRequest.TargetPosition);

    for (const auto & ChunkPosition : GetAdjascentChunkPositions(BlockChunkPosition))
    {
      if (entity_t ChunkEntity = Terrain.GetChunkAt(TargetChunkPosition + ChunkPosition); ChunkEntity != entt::null)
        Registry.get<TChunkComponent>(ChunkEntity).State = EChunkState::Dirty;
    }

    HitRequest.Status = ERequestStatus::Fulfilled;
  }
}


