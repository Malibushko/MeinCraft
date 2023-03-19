#pragma once
#include <glm/vec2.hpp>
#include <tsl/robin_map.h>
#include <glm/gtx/hash.hpp>

struct TTerrainBlockInfo
{
  float Continentalness{};
  float Erosion{};
  float PeaksValleys{};
  float Height{};
  float Temperature{};
  float Humidity{};
};

struct TTerrainMap
{
  tsl::robin_map<glm::ivec2, TTerrainBlockInfo> Blocks;
};