#include "FlatTerrainGenerator.h"

#include <unordered_map>

//
// Construction/Destruction
//

CFlatTerrainGenerator::CFlatTerrainGenerator(int Seed)
{
  // Empty
}

//
// Interface
//

TBlockComponent CFlatTerrainGenerator::Generate(glm::vec3 _Position)
{
  if (_Position.y == 0)
    return TBlockComponent{ EBlockType::GrassBlock };

  return TBlockComponent{ EBlockType::Air };
}
