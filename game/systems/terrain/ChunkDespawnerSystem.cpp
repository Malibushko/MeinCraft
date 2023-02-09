#include "ChunkDespawnerSystem.h"

#include "core/components/PositionComponent.h"
#include "game/components/terrain/BlockComponent.h"
#include "game/components/terrain/ChunkComponent.h"
#include "game/components/terrain/TerrainComponent.h"

//
// Construction/Destruction
//

CChunkDespawnerSystem::CChunkDespawnerSystem() = default;

CChunkDespawnerSystem::~CChunkDespawnerSystem() = default;

//
// ISystem
//

void CChunkDespawnerSystem::OnCreate(registry_t & Registry_)
{
  // Empty
}

void CChunkDespawnerSystem::OnUpdate(registry_t & Registry_, float Delta_)
{
 // Empty
}

void CChunkDespawnerSystem::OnDestroy(registry_t & Registry_)
{
  // Empty
}


