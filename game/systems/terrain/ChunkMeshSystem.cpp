#include "ChunkMeshSystem.h"

#include "game/components/terrain/BlockComponent.h"
#include "game/components/terrain/ChunkComponent.h"
#include "game/factory/BlockMeshFactory.h"

//
// Construction/Destruction
//

CChunkMeshSystem::CChunkMeshSystem() = default;

CChunkMeshSystem::~CChunkMeshSystem() = default;

//
// ISystem
//

void CChunkMeshSystem::OnCreate(registry_t & Registry_)
{
  // Empty
}

void CChunkMeshSystem::OnUpdate(registry_t & Registry_, float Delta_)
{
  static CBlockMeshFactory Factory;

  for (auto && [Entity, Chunk] : Registry_.view<TChunkComponent>().each())
  {
    if (Chunk.State == EChunkState::Clear)
      continue;

    for (const auto & BlockEntity : Chunk.Blocks)
    {
      if (auto * Block = Registry_.try_get<TBlockComponent>(BlockEntity); Block && Block->IsVisible())
        Factory.InitBlock(Registry_, BlockEntity);
    }

    Chunk.State = EChunkState::Clear;
  }
}

void CChunkMeshSystem::OnDestroy(registry_t & Registry_)
{
  // Empty
}

