#include "DesertBiomeGenerator.h"

static constexpr int MAX_HEIGHT = 4;

//
// IBiomeGenerator
//

EBlockType CDesertBiomeGenerator::GetBlockAt(int BaseHeight, int Elevation, int X_, int Y_)
{
  if (Elevation - BaseHeight < MAX_HEIGHT)
    return EBlockType::Sand;

  return EBlockType::Air;
}