#pragma once
#include <array>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "core/entity/Component.h"
#include "core/entity/entity.h"

enum class EChunkState
{
  Clear,
  Dirty
};

struct TChunkComponent
{
  static constexpr ComponentTag ComponentTag{};

  static constexpr int CHUNK_SIZE_X = 16;
  static constexpr int CHUNK_SIZE_Y = 16;
  static constexpr int CHUNK_SIZE_Z = 16;

  EChunkState State{EChunkState::Dirty};

  std::array<entity_t, CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z> Blocks;
};

inline glm::ivec2 ToChunkCoordinates(const glm::vec3 & Position_)
{
  return glm::ivec2(Position_.x / TChunkComponent::CHUNK_SIZE_X, Position_.z / TChunkComponent::CHUNK_SIZE_Z);
}

inline glm::vec3 FromChunkCoordinates(const glm::ivec2 & ChunkPosition_)
{
  return glm::vec3(
    ChunkPosition_.x * TChunkComponent::CHUNK_SIZE_X,
    0.0f,
    ChunkPosition_.y * TChunkComponent::CHUNK_SIZE_Z
  );
}