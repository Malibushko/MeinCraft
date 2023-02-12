#pragma once
#include "core/entity/System.h"

class CDirectedLightMovementSystem : public ISystem
{
public: // Construction/Destruction

  CDirectedLightMovementSystem();

  ~CDirectedLightMovementSystem() override;

public: // ISystem

  void OnCreate(registry_t & Registry_) override;

  void OnUpdate(registry_t & Registry_, float Delta_) override;

  void OnDestroy(registry_t & Registry_) override;

protected: // Members

  float m_MovementAccumulator{ 0.f };
  float m_PauseAccumulator{ 0.f };
};
