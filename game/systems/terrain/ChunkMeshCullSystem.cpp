#include "ChunkMeshCullSystem.h"

#include "core/components/TransformComponent.h"
#include "game/components/terrain/TerrainComponent.h"
#include "game/factory/BlockFactory.h"

//
// Construction/Destruction
//

CChunkMeshCullSystem::CChunkMeshCullSystem() = default;

CChunkMeshCullSystem::~CChunkMeshCullSystem() = default;

//
// ISystem
//

void CChunkMeshCullSystem::OnCreate(registry_t & Registry_)
{
  // Empty
}

void CChunkMeshCullSystem::OnUpdate(registry_t & Registry_, float Delta_)
{
  auto & Terrain = QuerySingle<TTerrainComponent>(Registry_);

  for (auto && [Entity, Chunk, Transform] : Registry_.view<TChunkComponent, TTransformComponent>().each())
  {
    if (Chunk.State == EChunkState::Clear)
      continue;

    const glm::ivec2 ChunkPosition = ToChunkCoordinates(glm::vec3(Transform.Transform[3]));

    UpdateBlocksFaces(Registry_, Terrain, ChunkPosition, Chunk);
  }
}

void CChunkMeshCullSystem::OnDestroy(registry_t& Registry_)
{
   // Empty
}

//
// Service
//

void CChunkMeshCullSystem::UpdateBlocksFaces(registry_t & Registry, TTerrainComponent & Terrain,
  const glm::ivec2 & ChunkPosition, TChunkComponent & Chunk)
{
  const auto GetBlockAt = [&](glm::ivec3 BlockPosition) -> std::pair<entity_t, bool>
  {
    entity_t ChunkEntity = entt::null;

    if (BlockPosition.x < 0)
    {
      ChunkEntity = Terrain.GetChunkAt(ChunkPosition + glm::ivec2(-1, 0));

      BlockPosition.x = TChunkComponent::CHUNK_SIZE_X - 1;
    }
    else if (BlockPosition.x > TChunkComponent::CHUNK_SIZE_X - 1)
    {
      ChunkEntity = Terrain.GetChunkAt(ChunkPosition + glm::ivec2(+1, 0));

      BlockPosition.x = 0;
    }
    else if (BlockPosition.z < 0)
    {
      ChunkEntity = Terrain.GetChunkAt(ChunkPosition + glm::ivec2(0, -1));

      BlockPosition.z = TChunkComponent::CHUNK_SIZE_Z - 1;
    }
    else if (BlockPosition.z > TChunkComponent::CHUNK_SIZE_Z - 1)
    {
      ChunkEntity = Terrain.GetChunkAt(ChunkPosition + glm::ivec2(0, +1));

      BlockPosition.z = 0;
    }
    else
    {
      ChunkEntity = Terrain.GetChunkAt(ChunkPosition);

      BlockPosition.y = std::clamp(BlockPosition.y, 0, TChunkComponent::CHUNK_SIZE_Y - 1);
    }

    if (ChunkEntity != entt::null)
      return { Registry.get<TChunkComponent>(ChunkEntity).GetBlockAt(BlockPosition), true };

    return { entt::null, false };
  };

  static const glm::ivec3 NeighbourOffsets[6] =
  {
    { 0, 0, +1},
    { 0, 0, -1},
    { -1, 0, 0},
    { +1, 0, 0},
    { 0, +1, 0},
    { 0, -1, 0},
  };

  for (size_t Index = 0; Index < TChunkComponent::BLOCKS_COUNT; Index++)
  {
    if (Chunk.Blocks[Index] == entt::null)
      continue;

    const glm::ivec3 BlockPosition = BlockIndexToPosition(Index);

    const bool IsThisBlockTransparent = CBlockFactory::IsBlockTransparent(Registry.get<TBlockComponent>(Chunk.Blocks[Index]));

    const auto IsBlockVisible = [&](entity_t BlockEntity) -> bool
    {
      if (IsThisBlockTransparent)
        return BlockEntity != entt::null;

      return BlockEntity != entt::null && !CBlockFactory::IsBlockTransparent(Registry.get<TBlockComponent>(BlockEntity));
    };

    TVisibleBlockFacesComponent Faces
    {
      .Faces = CBlockFactory::GetDefaultBlockMeshFaces(Registry.get<TBlockComponent>(Chunk.Blocks[Index]))
    };

    for (int i = 0; i < 6; i++)
    {
      const glm::vec3 NeighbourPosition = NeighbourOffsets[i] + BlockPosition;

      const auto [BlockEntity, IsChunkExist] = GetBlockAt(NeighbourPosition);

      if (IsChunkExist && !IsBlockVisible(BlockEntity))
        Faces.Faces = Faces.Faces | static_cast<EBlockFace>(1 << i);
    }

    Registry.emplace_or_replace<TVisibleBlockFacesComponent>(Chunk.Blocks[Index], Faces);
  }
}
