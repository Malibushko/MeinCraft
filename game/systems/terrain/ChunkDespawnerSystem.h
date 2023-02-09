#pragma once
#include "core/entity/System.h"

class CChunkDespawnerSystem : public ISystem
{
public: // Construction/Destruction

  CChunkDespawnerSystem();

  ~CChunkDespawnerSystem() override;

public: // ISystem

  void OnCreate(registry_t & Registry_) override;

  void OnUpdate(registry_t & Registry_, float Delta_) override;

  void OnDestroy(registry_t & Registry_) override;
};