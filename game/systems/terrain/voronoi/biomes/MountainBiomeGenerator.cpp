#include "MountainBiomeGenerator.h"

static constexpr int MAX_HEIGHT = 7;

//
// IBiomeGenerator
//

EBlockType CMountainBiomeGenerator::GetBlockAt(int BaseHeight, int Elevation, int X_, int Y_)
{
  if (Elevation - BaseHeight > MAX_HEIGHT)
    return EBlockType::Air;

  return EBlockType::Stone;
}