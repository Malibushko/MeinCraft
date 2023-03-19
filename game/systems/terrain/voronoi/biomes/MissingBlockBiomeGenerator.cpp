#include "MissingBlockBiomeGenerator.h"

//
// IBiomeGenerator
//

EBlockType CMissingBlockBiomeGenerator::GetBlockAt(int Elevation, int X_, int Y_)
{
  return EBlockType::Missing;
}
