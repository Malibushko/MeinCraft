#pragma once
#include <glm/vec3.hpp>

#include "core/entity/System.h"

class CInventorySystem : public ISystem
{
public: // Construction/Destruction

  CInventorySystem();

  ~CInventorySystem();

public: // ISystem

  void OnCreate(registry_t& Registry_) override;

  void OnUpdate(registry_t& Registry_, float Delta_) override;

  void OnDestroy(registry_t& Registry_) override;
};
