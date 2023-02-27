#pragma once
#include <glm/vec3.hpp>

#include "core/entity/System.h"

class CPhysicsSystem : public ISystem
{
public: // Construction/Destruction

  CPhysicsSystem();

  ~CPhysicsSystem();

public: // Interface

  void OnCreate(registry_t & Registry_) override;

  void OnUpdate(registry_t & Registry_, float Delta_) override;

  void OnDestroy(registry_t & Registry_) override;

protected: // Service

  void ProcessRaycastRequests(registry_t & Registry);

  std::pair<entity_t, glm::vec3> Raycast(registry_t & Registry, glm::vec3 StartPosition, glm::vec3 Direction, float Distance) const;

  entity_t GetEntityAt(registry_t & Registry, glm::vec3 Position) const;

  glm::vec3 CalculateDefaultRayPosition(registry_t & Registry) const;

  glm::vec3 CalculateDefaultRayDirection(registry_t & Registry) const;
};
