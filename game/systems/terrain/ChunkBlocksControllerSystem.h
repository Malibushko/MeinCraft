#pragma once
#include "core/entity/System.h"

class CChunkBlocksControllerSystem : public ISystem
{
public: // Construction/Destruction

  CChunkBlocksControllerSystem();

  ~CChunkBlocksControllerSystem() override;

public: // ISystem

  void OnCreate(registry_t & Registry_) override;

  void OnUpdate(registry_t & Registry_, float Delta_) override;

  void OnDestroy(registry_t & Registry_) override;

protected: // Service

  void ProcessRequests(registry_t & _Registry);

  void ProcessBlockCreateRequests(registry_t & Registry);

  void ProcessBlockHitRequests(registry_t & Registry);
};