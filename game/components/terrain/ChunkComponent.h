#pragma once
#include <array>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/gtx/compatibility.hpp>

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
  static constexpr int CHUNK_SIZE_Y = 384;
  static constexpr int CHUNK_SIZE_Z = 16;

  EChunkState State{EChunkState::Dirty};

  std::array<entity_t, CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z> Blocks;

  std::vector<entity_t> Meshes;

  entity_t GetBlockAt(glm::ivec3 ChunkPosition) const
  {
    return Blocks[ChunkPosition.x + CHUNK_SIZE_X * (ChunkPosition.y + CHUNK_SIZE_Y * ChunkPosition.z)];
  }

  entity_t & GetBlockAt(glm::ivec3 ChunkPosition)
  {
    return Blocks[ChunkPosition.x + CHUNK_SIZE_X * (ChunkPosition.y + CHUNK_SIZE_Y * ChunkPosition.z)];
  }
};

inline glm::ivec2 ToChunkCoordinates(const glm::vec3 & Position_)
{
  const int X = static_cast<int>(std::floor(Position_.x / TChunkComponent::CHUNK_SIZE_X));
  const int Y = static_cast<int>(std::floor(Position_.z / TChunkComponent::CHUNK_SIZE_Z));

  return { X, Y };
}

inline glm::vec3 FromChunkCoordinates(const glm::ivec2 & ChunkPosition_)
{
  return glm::vec3(
    ChunkPosition_.x * TChunkComponent::CHUNK_SIZE_X,
    0.0f,
    ChunkPosition_.y * TChunkComponent::CHUNK_SIZE_Z
  );
}

inline glm::ivec3 WorldToChunkPosition(glm::vec3 Position)
{
  int X = static_cast<int>(std::floor(Position.x)) % TChunkComponent::CHUNK_SIZE_X;
  int Y = static_cast<int>(std::floor(Position.y)) % TChunkComponent::CHUNK_SIZE_Y;
  int Z = static_cast<int>(std::floor(Position.z)) % TChunkComponent::CHUNK_SIZE_Z;

  if (X < 0) X += TChunkComponent::CHUNK_SIZE_X;
  if (Y < 0) Y += TChunkComponent::CHUNK_SIZE_Y;
  if (Z < 0) Z += TChunkComponent::CHUNK_SIZE_Z;

  return glm::ivec3(X, Y, Z);
}

inline std::vector<glm::ivec2> GetAdjascentChunkPositions(glm::ivec3 BlockPosition)
{
  std::vector<glm::ivec2> Result;

  if (BlockPosition.x == 0)
    Result.emplace_back(-1, 0);

  if (BlockPosition.x == TChunkComponent::CHUNK_SIZE_X - 1)
    Result.emplace_back(1, 0);

  if (BlockPosition.z == 0)
    Result.emplace_back(0, -1);

  if (BlockPosition.z == TChunkComponent::CHUNK_SIZE_Z - 1)
    Result.emplace_back(0, 1);

  return Result;
}