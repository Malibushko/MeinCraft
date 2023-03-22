#include "OceanBiomeGenerator.h"

static constexpr float WATER_LEVEL = 10;

//
// IBiomeGenerator
//

EBlockType COceanBiomeGenerator::GetBlockAt(int BaseHeight, int Elevation, int X_, int Y_)
{
  if (Elevation < WATER_LEVEL)
    return EBlockType::StationaryWater;

  return EBlockType::Air;
}