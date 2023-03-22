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

  int GetErosionIndex() const
  {
    if (Erosion < 0.2f)
      return 0;

    if (Erosion < 0.2225f)
      return 1;

    if (Erosion < 0.3f)
      return 2;

    if (Erosion < 0.45f)
      return 3;

    if (Erosion < 0.55f)
      return 4;

    if (Erosion < 0.78f)
      return 5;

    return 6;
  }

  int GetContinentalnessIndex() const
  {
    if (Continentalness < 0.2f)
      return 0;

    if (Continentalness < 0.45f)
      return 1;

    if (Continentalness < 0.65f)
      return 2;

    if (Continentalness < 0.75f)
      return 3;

    return 4;
  }
};

struct TTerrainMap
{
  tsl::robin_map<glm::ivec2, TTerrainBlockInfo> Blocks;
};