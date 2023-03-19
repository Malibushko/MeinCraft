#include "MountainBiomeGenerator.h"

//
// IBiomeGenerator
//

EBlockType CMountainBiomeGenerator::GetBlockAt(int Elevation, int X_, int Y_)
{
  return EBlockType::Stone;
}