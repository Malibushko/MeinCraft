#pragma once
#include <set>

#include "core/entity/System.h"
#include "game/components/render/GLShaderComponent.h"
#include <glad/glad.h>

enum class EShaderBuffer
{
  MatricesBuffer,
  CameraBuffer,
  DirectedLightBuffer,
  PointLightsBuffer,
  VisiblePointLightsIndicesBuffer
};

class GLRenderBufferObjectsSystem : public ISystem
{
public: // ISystem

  void OnCreate(registry_t & Registry_) override;

  void OnUpdate(registry_t & Registry_, float Delta_) override;

  void OnDestroy(registry_t & Registry_) override;

protected: // Service

  void UpdateStorageBuffers(registry_t & Registry_);

  void UpdateMatricesBuffer(registry_t & Registry_);

  void UpdateCameraBuffer(registry_t & Registry_);

  void UpdateDirectedLightBuffer(registry_t & Registry_);

  void UpdatePointLightsBuffer(registry_t & Registry);

protected: // Service

  std::set<GLuint> m_UpdatedShaders;
};
