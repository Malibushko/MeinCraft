#pragma once
#include "core/entity/System.h"
#include "game/components/render/GLUnbakedMeshComponent.h"

struct TTerrainComponent;
struct TChunkComponent;
struct TBlockComponent;

class CChunkMeshCullSystem : public ISystem
{
public: // Construction/Destruction

  CChunkMeshCullSystem();

  ~CChunkMeshCullSystem() override;

public: // ISystem

  void OnCreate(registry_t & Registry_) override;

  void OnUpdate(registry_t & Registry_, float Delta_) override;

  void OnDestroy(registry_t & Registry_) override;

protected: // Service

  void UpdateBlocksFaces(registry_t & Registry, TTerrainComponent & Terrain, const glm::ivec2 & ChunkPosition, TChunkComponent & Chunk);
};
