#include "ChunkSpawnerSystem.h"

#include "core/components/PositionComponent.h"

#include "game/components/camera/CameraBasisComponent.h"
#include "game/components/terrain/BlockComponent.h"
#include "game/components/terrain/ChunkComponent.h"
#include "game/components/terrain/TerrainComponent.h"
#include "game/components/terrain/VisibleBlockFacesComponent.h"

//
// Config
//

static constexpr int        CHUNK_SPAWN_DISTANCE = 1;
static constexpr std::array DIRECTIONS           =
{
  glm::ivec2(-1, 1),  glm::ivec2(0, 1),  glm::ivec2(1, 1),
  glm::ivec2(-1, 0),  glm::ivec2(0, 0),  glm::ivec2(1, 0),
  glm::ivec2(-1, -1), glm::ivec2(0, -1), glm::ivec2(1, -1)
};

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
  auto & Terrain     = QueryFirst<TTerrainComponent>(Registry_);
  auto   CameraViews = Registry_.view<TGlobalTransformComponent, TPositionComponent>().each();

  for (auto && [Entity, Transform, Position] : CameraViews)
  {
    glm::ivec2 ChunkPosition = ToChunkCoordinates(Position.Position);

    for (int Distance = 0; Distance < CHUNK_SPAWN_DISTANCE; Distance++)
    {
      for (glm::ivec2 Direction : DIRECTIONS)
      {
        Direction *= Distance;

        if (!Terrain.Chunks.contains(ChunkPosition + Direction))
          SpawnChunkAt(Registry_, Terrain, ChunkPosition + Direction);
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

        const auto [BlockEntity, Block] = Create<TBlockComponent>(Registry_);

        Block = Terrain.TerrainGenerationStrategy(BlockPosition);

        AddComponent(Registry_, BlockEntity, TPositionComponent{ .Position = BlockPosition });

        Chunk.Blocks[Index] = BlockEntity;
      }
    }
  }

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

        TVisibleBlockFacesComponent Faces{ .Faces = EBlockFace::None };

        if (X == 0)
          Faces.Faces = Faces.Faces | EBlockFace::Front;

        if (X == TChunkComponent::CHUNK_SIZE_X - 1)
          Faces.Faces = Faces.Faces | EBlockFace::Back;

        if (Y == 0)
          Faces.Faces = Faces.Faces | EBlockFace::Bottom;

        if (Y == TChunkComponent::CHUNK_SIZE_Y - 1)
          Faces.Faces = Faces.Faces | EBlockFace::Top;

        if (Z == 0)
          Faces.Faces = Faces.Faces | EBlockFace::Left;

        if (Z == TChunkComponent::CHUNK_SIZE_Z - 1)
          Faces.Faces = Faces.Faces | EBlockFace::Right;

        Registry.emplace<TVisibleBlockFacesComponent>(Chunk.Blocks[Index], Faces);
      }
    }
  }
}


