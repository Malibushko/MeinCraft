#pragma once
#include <set>

#include "core/entity/System.h"
#include "game/components/render/GLShaderComponent.h"
#include <glad/glad.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

enum class EShaderBuffer
{
  MatricesBuffer,
  CameraBuffer,
  DirectedLightBuffer,
  PointLightsBuffer,
  VisiblePointLightsIndicesBuffer,
  MaterialsBuffer,
  TerrainBuffer
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

  void UpdateMaterialsBuffer(registry_t & Registry);

  void UpdateTerrainBuffer(registry_t & Registry);

  void RebuildTerrainMap(registry_t & Registry);

  void SetTerrainMapValue(registry_t & Registry, glm::vec3 WorldPosition, GLubyte Value);

protected: // Service events

  void OnChunkCreated(registry_t & Registry, glm::ivec2 ChunkCoordinates);

  void OnChunkBlockCreated(registry_t & Registry, glm::ivec2 ChunkCoordinates, glm::ivec3 BlockCoordinates);

  void OnChunkBlockDeleted(registry_t & Registry, glm::ivec2 ChunkCoordinates, glm::ivec3 BlockCoordinates);

protected: // Service

  std::set<GLuint> m_UpdatedShaders;
  bool m_NeedToUpdateTerrainTexture{false};
};
