#pragma once
#include "core/entity/System.h"

class CRequestsSystem : public ISystem
{
public: // Construction/Destruction

  CRequestsSystem() = default;

  ~CRequestsSystem() override = default;

public: // Interface

  void OnCreate(registry_t & Registry_) override;

  void OnUpdate(registry_t & Registry_, float Delta_) override;

  void OnDestroy(registry_t & Registry_) override;
};
