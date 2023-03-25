#pragma once
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

enum class EChunkChangeType
{
  BlockCreated,
  BlockDeleted
};

struct TChunkChangedEvent
{
  glm::ivec2  ChunkCoordinates;

  EChunkChangeType ChangeType;
  glm::ivec3  ChangedBlockCoordinates;
};
