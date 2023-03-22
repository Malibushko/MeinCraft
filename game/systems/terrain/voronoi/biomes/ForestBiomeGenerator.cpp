#include "ForestBiomeGenerator.h"

static constexpr int MAX_HEIGHT = 4;

//
// IBiomeGenerator
//

EBlockType CForestBiomeGenerator::GetBlockAt(int BaseHeight, int Elevation, int X_, int Y_)
{
  if (Elevation - BaseHeight > MAX_HEIGHT)
    return EBlockType::Air;

  if (Elevation - BaseHeight == MAX_HEIGHT)
    return EBlockType::Dirt;

  return EBlockType::GrassBlock;
}