#pragma once
#include "core/entity/System.h"
#include "game/components/render/GLUnbakedMeshComponent.h"

struct TChunkComponent;
struct TBlockComponent;

class CChunkMeshSystem : public ISystem
{
public: // Construction/Destruction

  CChunkMeshSystem();

  ~CChunkMeshSystem() override;

public: // ISystem

  void OnCreate(registry_t & Registry_) override;

  void OnUpdate(registry_t & Registry_, float Delta_) override;

  void OnDestroy(registry_t & Registry_) override;

protected: // Service

  void RecreateChunkMesh(registry_t & Registry_, entity_t ChunkEntity, TChunkComponent & Chunk) const;

  void DestroyChunkMesh(registry_t & Registry, TChunkComponent & Chunk) const;
};
