#include "ChunkSpawnerSystem.h"

#include <spdlog/spdlog.h>

#include "core/components/PositionComponent.h"
#include "core/components/TransformComponent.h"

#include "game/components/camera/CameraBasisComponent.h"
#include "game/components/physics/BoundingVolume.h"
#include "game/components/terrain/BlockComponent.h"
#include "game/components/terrain/ChunkComponent.h"
#include "game/components/terrain/TerrainComponent.h"
#include "game/components/terrain/VisibleBlockFacesComponent.h"

//
// Config
//

static constexpr int CHUNK_SPAWN_RADIUS = 47;

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

  for (auto && [Entity, Transform, Position] : CameraViews)
  {
    glm::ivec2 ChunkPosition = ToChunkCoordinates(Position.Position);

    for (int X = -CHUNK_SPAWN_RADIUS / 2; X < CHUNK_SPAWN_RADIUS / 2; X++)
    {
      for (int Y = -CHUNK_SPAWN_RADIUS / 2; Y < CHUNK_SPAWN_RADIUS / 2; Y++)
      {
        if (!Terrain.Chunks.contains(ChunkPosition + glm::ivec2(X, Y)))
          SpawnChunkAt(Registry_, Terrain, ChunkPosition + glm::ivec2(X, Y));
      }
    }
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

  for (int X = 0; X < TChunkComponent::CHUNK_SIZE_X; X++)
  {
    for (int Y = 0; Y < TChunkComponent::CHUNK_SIZE_Y; Y++)
    {
      for (int Z = 0; Z < TChunkComponent::CHUNK_SIZE_Z; Z++)
      {
        const int        Index           = X + TChunkComponent::CHUNK_SIZE_X * (Y + TChunkComponent::CHUNK_SIZE_Y * Z);
        const glm::vec3  BlockPosition   = FromChunkCoordinates(ChunkPosition) + glm::vec3(X, Y, Z);

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
    }
  }

  TTransformComponent ChunkTransform
  {
    .Transform = glm::translate(glm::mat4(1.0f), FromChunkCoordinates(ChunkPosition))
  };

  AddComponent(Registry_, Entity, std::move(ChunkTransform));

  UpdateBlocksFaces(Registry_, Chunk);

  Terrain.Chunks[ChunkPosition] = Entity;
}

void CChunkSpawnerSystem::UpdateBlocksFaces(registry_t & Registry, TChunkComponent & Chunk)
{
  for (int X = 0; X < TChunkComponent::CHUNK_SIZE_X; X++)
  {
    for (int Y = TChunkComponent::CHUNK_SIZE_Y - 1; Y >= 0; Y--)
    {
      for (int Z = 0; Z < TChunkComponent::CHUNK_SIZE_Z; Z++)
      {
        const int Index = X + TChunkComponent::CHUNK_SIZE_X * (Y + TChunkComponent::CHUNK_SIZE_Y * Z);

        const int NeighbourIndices[6] =
        {
          X - 1 + TChunkComponent::CHUNK_SIZE_X * (Y + TChunkComponent::CHUNK_SIZE_Y * Z),
          X + 1 + TChunkComponent::CHUNK_SIZE_X * (Y + TChunkComponent::CHUNK_SIZE_Y * Z),
          X + TChunkComponent::CHUNK_SIZE_X * (Y - 1 + TChunkComponent::CHUNK_SIZE_Y * Z),
          X + TChunkComponent::CHUNK_SIZE_X * (Y + 1 + TChunkComponent::CHUNK_SIZE_Y * Z),
          X + TChunkComponent::CHUNK_SIZE_X * (Y + TChunkComponent::CHUNK_SIZE_Y * (Z - 1)),
          X + TChunkComponent::CHUNK_SIZE_X * (Y + TChunkComponent::CHUNK_SIZE_Y * (Z + 1))
        };

        if (Chunk.Blocks[Index] == entt::null)
          continue;

        TVisibleBlockFacesComponent Faces{ .Faces = EBlockFace::None };

        if (X == 0 || Chunk.Blocks[NeighbourIndices[0]] == entt::null)
          Faces.Faces = Faces.Faces | EBlockFace::Front;

        if (X == TChunkComponent::CHUNK_SIZE_X - 1 || Chunk.Blocks[NeighbourIndices[1]] == entt::null)
          Faces.Faces = Faces.Faces | EBlockFace::Back;

        if (Y == 0 || Chunk.Blocks[NeighbourIndices[2]] == entt::null)
          Faces.Faces = Faces.Faces | EBlockFace::Bottom;

        if (Y == TChunkComponent::CHUNK_SIZE_Y - 1 || Chunk.Blocks[NeighbourIndices[3]] == entt::null)
          Faces.Faces = Faces.Faces | EBlockFace::Top;

        if (Z == 0 || Chunk.Blocks[NeighbourIndices[4]] == entt::null)
          Faces.Faces = Faces.Faces | EBlockFace::Left;

        if (Z == TChunkComponent::CHUNK_SIZE_Z - 1 || Chunk.Blocks[NeighbourIndices[5]] == entt::null)
          Faces.Faces = Faces.Faces | EBlockFace::Right;

        Registry.emplace<TVisibleBlockFacesComponent>(Chunk.Blocks[Index], Faces);
      }
    }
  }
}


