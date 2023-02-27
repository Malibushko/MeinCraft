#pragma once
#include <glm/vec3.hpp>

#include "core/entity/System.h"

struct TCameraTargetComponent;

class CTargetBlockHighlightSystem : public ISystem
{
public: // Construction/Destruction

  CTargetBlockHighlightSystem();

  ~CTargetBlockHighlightSystem();

public: // ISystem

  void OnCreate(registry_t& Registry_) override;

  void OnUpdate(registry_t& Registry_, float Delta_) override;

  void OnDestroy(registry_t& Registry_) override;

protected: // Service

  void UpdateTarget(registry_t & Registry, TCameraTargetComponent & Target);

protected: // Members

  entity_t m_MeshEntity{entt::null};
};
