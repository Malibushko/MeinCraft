#pragma once
#include "core/entity/System.h"

class CFPSCounterSystem : public ISystem
{
public: // Construction/Destruction

  CFPSCounterSystem();

  ~CFPSCounterSystem() override;

public: // ISystem

  void OnCreate(registry_t & Registry_) override;

  void OnDestroy(registry_t & Registry_) override;

  void OnUpdate(registry_t & Registry_, float Delta_) override;

private: // Members

  float m_DeltaAccumulator{};
  int m_FramesCount{ 0 };
};
