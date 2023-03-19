#include "DesertBiomeGenerator.h"

//
// IBiomeGenerator
//

EBlockType CDesertBiomeGenerator::GetBlockAt(int Elevation, int X_, int Y_)
{
  return EBlockType::Sand;
}