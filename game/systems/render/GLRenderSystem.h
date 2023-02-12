#pragma once
#include <glad/glad.h>

#include "core/entity/System.h"
#include "game/components/camera/CameraBasisComponent.h"
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

  void UpdateFrustum(const TGlobalTransformComponent & Transform);

  void UpdateUniformBlocks(registry_t & Registry_);

  bool IsNeedUpdateFrustum(registry_t & Registry) const;

  void UpdateMatricesUBO(registry_t & Registry_);

  void UpdateCameraUBO(registry_t & Registry_);

  void UpdateLightUBO(registry_t & Registry_);

protected: // Members

  CFrustum m_RenderFrustum;

  bool m_IsNeedUpdateFrustum{ true };

  GLuint m_MatricesUBO{ 0 };
  GLuint m_LightUBO{ 0 };
  GLuint m_CameraUBO{ 0 };
};
