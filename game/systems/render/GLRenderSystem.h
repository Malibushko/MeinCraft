#pragma once
#include <glad/glad.h>

#include "core/entity/System.h"
#include "game/components/camera/CameraBasisComponent.h"
#include "game/components/render/GLShaderComponent.h"
#include "game/math/Frustum.h"

struct TGLShaderComponent;

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

  bool IsNeedUpdateFrustum(registry_t & Registry) const;
};
