#include "ChunkBlocksControllerSystem.h"

#include "core/components/PositionComponent.h"
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

    // почему то не обновляются блоки у соседних чанков
    for (const auto & ChunkPosition : GetAdjascentChunkPositions(BlockChunkPosition))
    {
      if (entity_t ChunkEntity = Terrain.GetChunkAt(TargetChunkPosition + ChunkPosition); ChunkEntity != entt::null)
        Registry.get<TChunkComponent>(ChunkEntity).State = EChunkState::Dirty;
    }

    if (HitRequest.DestroyWhenFulfilled)
      Registry.destroy(Entity);
    else
      HitRequest.IsFulfilled = true;
  }
}


