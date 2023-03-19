#include "OceanBiomeGenerator.h"

//
// IBiomeGenerator
//

EBlockType COceanBiomeGenerator::GetBlockAt(int Elevation, int X_, int Y_)
{
  return EBlockType::StationaryWater;
}