#include "OceanBiomeGenerator.h"

//
// IBiomeGenerator
//

EBlockType COceanBiomeGenerator::GetBlockAt(int BaseHeight, int Elevation, int X_, int Y_)
{
  return EBlockType::StationaryWater;
}