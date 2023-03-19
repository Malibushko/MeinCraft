#include "PlainsBiomeGenerator.h"

static constexpr int MAX_HEIGHT = 2;

//
// IBiomeGenerator
//

EBlockType CPlainsBiomeGenerator::GetBlockAt(int BaseHeight, int Elevation, int X_, int Y_)
{
  const auto BlockElevation = Elevation - BaseHeight;

  if (BlockElevation < MAX_HEIGHT)
    return EBlockType::Dirt;

  if (BlockElevation == MAX_HEIGHT)
    return EBlockType::GrassBlock;

  return EBlockType::Air;
}