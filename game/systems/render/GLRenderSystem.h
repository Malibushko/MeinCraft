#pragma once
#include "core/entity/System.h"
#include "game/math/Frustum.h"

class GLRenderSystem : public ISystem
{
public: // Construction/Destruction

  GLRenderSystem();

  ~GLRenderSystem() override;

public: // ISystem

  void OnCreate(registry_t & Registry_) override;

  void OnUpdate(registry_t & Registry_, float Delta_) override;

  void OnDestroy(registry_t & Registry_) override;

protected: // Service

  void UpdateFrustum(const TGlobalTransformComponent & Transform);

  bool IsNeedUpdateFrustum(registry_t & Registry) const;

protected: // Members

  CFrustum m_RenderFrustum;

  bool m_IsNeedUpdateFrustum{ true };
};
