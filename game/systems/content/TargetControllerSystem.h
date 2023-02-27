#pragma once
#include <glm/vec3.hpp>

#include "core/entity/System.h"

class CTargetControllerSystem : public ISystem
{
public: // Construction/Destruction

  CTargetControllerSystem();

  ~CTargetControllerSystem();

public: // ISystem

  void OnCreate(registry_t& Registry_) override;

  void OnUpdate(registry_t& Registry_, float Delta_) override;

  void OnDestroy(registry_t& Registry_) override;

protected: // Members

  bool m_NeedToUpdateTarget{true};
};
