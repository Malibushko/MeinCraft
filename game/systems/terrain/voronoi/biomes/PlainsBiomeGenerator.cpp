#include "PlainsBiomeGenerator.h"

//
// IBiomeGenerator
//

EBlockType CPlainsBiomeGenerator::GetBlockAt(int Elevation, int X_, int Y_)
{
  return EBlockType::GrassBlock;
}