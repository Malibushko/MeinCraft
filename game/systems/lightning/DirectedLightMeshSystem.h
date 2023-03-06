#pragma once
#include "core/entity/System.h"

class CDirectedLightMeshSystem : public ISystem
{
public: // ISystem

  void OnCreate(registry_t & Registry_) override;

  void OnUpdate(registry_t & Registry_, float Delta_) override;

  void OnDestroy(registry_t & Registry_) override;

protected: // Members

  entity_t m_MeshEntity{ entt::null };
};
