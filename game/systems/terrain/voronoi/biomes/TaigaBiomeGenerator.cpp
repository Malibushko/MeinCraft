#include "TaigaBiomeGenerator.h"

static constexpr float MAX_HEIGHT = 3;

//
// IBiomeGenerator
//

EBlockType CTaigaBiomeGenerator::GetBlockAt(int BaseHeight, int Elevation, int X_, int Y_)
{
  if (Elevation - BaseHeight > MAX_HEIGHT)
    return EBlockType::Air;

  return EBlockType::Dirt;
}