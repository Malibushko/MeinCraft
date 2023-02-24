#pragma once
#include <glm/gtx/hash.hpp>
#include <unordered_set>
#include <glm/vec3.hpp>

#include "game/components/terrain/BlockComponent.h"
class CFlatTerrainGenerator
{
public: // Construction/Destruction

  CFlatTerrainGenerator(int Seed = 0);

  ~CFlatTerrainGenerator() = default;

public: // Interface

  TBlockComponent Generate(glm::vec3 _Position);
};
