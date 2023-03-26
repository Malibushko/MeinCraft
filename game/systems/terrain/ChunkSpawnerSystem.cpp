#include "ChunkSpawnerSystem.h"

#include <set>
#include <unordered_set>
#include <spdlog/spdlog.h>

#include "core/components/PositionComponent.h"
#include "core/components/TransformComponent.h"

#include "game/components/camera/CameraBasisComponent.h"
#include "game/components/physics/BoundingVolume.h"
#include "game/components/terrain/BlockComponent.h"
#include "game/components/terrain/ChunkComponent.h"
#include "game/components/terrain/TerrainComponent.h"
#include "game/components/terrain/VisibleBlockFacesComponent.h"
#include "game/factory/BlockFactory.h"

//
// Config
//

static constexpr int CHUNK_SPAWN_RADIUS = 5;

static_assert((CHUNK_SPAWN_RADIUS & 1) && "Spawn Distance must be odd!");

//
// Construction/Destruction
//

CChunkSpawnerSystem::CChunkSpawnerSystem() = default;

CChunkSpawnerSystem::~CChunkSpawnerSystem() = default;

//
// ISystem
//

void CChunkSpawnerSystem::OnCreate(registry_t & Registry_)
{
  // Empty
}

void CChunkSpawnerSystem::OnUpdate(registry_t & Registry_, float Delta_)
{
  auto & Terrain     = QuerySingle<TTerrainComponent>(Registry_);
  auto   CameraViews = Registry_.view<TGlobalTransformComponent, TPositionComponent>().each();

  std::unordered_set<glm::ivec2> ChunksToUpdate;

    for (auto && [Entity, Transform, Position] : CameraViews)
    {
      glm::ivec2 PlayerPosition = ToChunkCoordinates(Position.Position);

      for (int X = -CHUNK_SPAWN_RADIUS / 2; X < CHUNK_SPAWN_RADIUS / 2; X++)
      {
        for (int Y = -CHUNK_SPAWN_RADIUS / 2; Y < CHUNK_SPAWN_RADIUS / 2; Y++)
        {
          const glm::ivec2 ChunkPosition = PlayerPosition + glm::ivec2(X, Y);

          if (!Terrain.Chunks.contains(ChunkPosition))
          {
            SpawnChunkAt(Registry_, Terrain, ChunkPosition);

            ChunksToUpdate.insert(ChunkPosition + glm::ivec2(1, 0));
            ChunksToUpdate.insert(ChunkPosition + glm::ivec2(-1, 0));
            ChunksToUpdate.insert(ChunkPosition + glm::ivec2(0, 1));
            ChunksToUpdate.insert(ChunkPosition + glm::ivec2(0, -1));
          }
        }
      }
    }

  for (auto & ChunkPosition : ChunksToUpdate)
  {
    const entity_t ChunkEntity = Terrain.GetChunkAt(ChunkPosition);

    if (ChunkEntity != entt::null)
      GetComponent<TChunkComponent>(Registry_, ChunkEntity).State = EChunkState::Dirty;
  }
}

void CChunkSpawnerSystem::OnDestroy(registry_t & Registry_)
{
  // Empty
}

//
// Service
//

void CChunkSpawnerSystem::SpawnChunkAt(
    registry_t        & Registry_,
    TTerrainComponent & Terrain,
    const glm::ivec2  & ChunkPosition
  )
{
  auto && [Entity, Chunk] = Create<TChunkComponent>(Registry_);

  for (int Index = 0; Index < TChunkComponent::BLOCKS_COUNT; Index++)
  {
    const glm::vec3 BlockPosition = FromChunkCoordinates(ChunkPosition) + BlockIndexToPosition(Index);

    TBlockComponent Block = Terrain.TerrainGenerationStrategy(BlockPosition);

    if (!Block.IsVisible())
    {
      Chunk.Blocks[Index] = entt::null;

      continue;
    }

    const auto [BlockEntity, BlockComponent] = Create<TBlockComponent>(Registry_);

    BlockComponent = Block;

    Chunk.Blocks[Index] = BlockEntity;
  }

  TTransformComponent ChunkTransform
  {
    .Transform = glm::translate(glm::mat4(1.0f), glm::vec3(FromChunkCoordinates(ChunkPosition)))
  };

  AddComponent(Registry_, Entity, std::move(ChunkTransform));

  Terrain.Chunks[ChunkPosition] = Entity;
}

