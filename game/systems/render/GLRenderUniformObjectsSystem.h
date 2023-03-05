#pragma once
#include <set>

#include "core/entity/System.h"
#include "game/components/render/GLShaderComponent.h"
#include <glad/glad.h>

enum class EUniformBlock
{
  MatricesBlock,
  CameraBlock,
  LightBlock
};

class GLRenderUniformObjectsSystem : public ISystem
{
public: // ISystem

  void OnCreate(registry_t & Registry_) override;

  void OnUpdate(registry_t & Registry_, float Delta_) override;

  void OnDestroy(registry_t & Registry_) override;

protected: // Service

  void UpdateUniformBlocks(registry_t & Registry_);

  void UpdateMatricesUBO(registry_t & Registry_);

  void UpdateCameraUBO(registry_t & Registry_);

  void UpdateLightUBO(registry_t & Registry_);

public: // Events

  void OnShaderCreated(registry_t & Registry, entity_t Entity);

protected: // Service

  std::set<GLuint> m_UpdatedShaders;
};
