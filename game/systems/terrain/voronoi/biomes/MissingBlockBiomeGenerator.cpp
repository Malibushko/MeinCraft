#include "MissingBlockBiomeGenerator.h"

//
// IBiomeGenerator
//

EBlockType CMissingBlockBiomeGenerator::GetBlockAt(int Elevation, int BaseHeight, int X_, int Y_)
{
  return EBlockType::Air;
}
