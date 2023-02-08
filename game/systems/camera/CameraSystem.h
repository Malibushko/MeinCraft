#pragma once
#include "core/entity/System.h"

class CCameraSystem : public ISystem
{
public: // Construction/Destruction

  CCameraSystem();

  ~CCameraSystem() override;

public: // ISystem

  void OnCreate(registry_t & Registry_) override;

  void OnUpdate(registry_t & Registry_, float Delta_) override;

  void OnDestroy(registry_t & Registry_) override;
};