#pragma once
#include <glm/vec2.hpp>

#include "core/entity/System.h"
#include "game/components/terrain/ChunkComponent.h"

struct TTerrainComponent;

class CChunkSpawnerSystem : public ISystem
{
public: // Construction/Destruction

  CChunkSpawnerSystem();

  ~CChunkSpawnerSystem() override;

public: // ISystem

  void OnCreate(registry_t & Registry_) override;

  void OnUpdate(registry_t & Registry_, float Delta_) override;

  void OnDestroy(registry_t & Registry_) override;

protected: // Service

  void SpawnChunkAt(registry_t & Registry_, TTerrainComponent & Terrain, const glm::ivec2 & ChunkPosition);
};
