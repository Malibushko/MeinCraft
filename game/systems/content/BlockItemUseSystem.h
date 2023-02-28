#pragma once
#include <glm/vec3.hpp>

#include "core/entity/System.h"

class CBlockItemUseSystem : public ISystem
{
public: // Construction/Destruction

  CBlockItemUseSystem();

  ~CBlockItemUseSystem();

public: // ISystem

  void OnCreate(registry_t& Registry_) override;

  void OnUpdate(registry_t& Registry_, float Delta_) override;

  void OnDestroy(registry_t& Registry_) override;

protected: // Service

  void ProcessSentRequests(registry_t & Registry);
};
