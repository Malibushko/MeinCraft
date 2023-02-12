#pragma once
#include "core/entity/System.h"

class CWorldLightMeshSystem : public ISystem
{
public: // Construction/Destruction

  CWorldLightMeshSystem();

  ~CWorldLightMeshSystem() override;

public: // ISystem

  void OnCreate(registry_t & Registry_) override;

  void OnUpdate(registry_t & Registry_, float Delta_) override;

  void OnDestroy(registry_t & Registry_) override;

protected: // Service

  void InitWorldLightMesh(registry_t & Registry, entity_t LightEntity);
};
