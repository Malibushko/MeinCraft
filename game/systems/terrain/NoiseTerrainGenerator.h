#pragma once
#include <glm/gtx/hash.hpp>
#include <unordered_set>
#include <glm/vec3.hpp>

#include "game/components/terrain/BlockComponent.h"
#include "game/utils/LRUCache.h"
#include "third_party/FastNoise/FastNoiseLite.h"

class CNoiseTerrainGenerator
{
public: // Construction/Destruction

  CNoiseTerrainGenerator(int Seed = 0);

  ~CNoiseTerrainGenerator() = default;

public: // Interface

  TBlockComponent Generate(glm::vec3 _Position);

protected: // Service

  TBlockComponent GenerateImpl(glm::vec3 _Position);

  float GetNoise(float x, float y);

  void SpawnTreeAt(glm::vec3 _Position);

private: // Members

  FastNoiseLite                                  m_Noise;
  std::unordered_map<glm::vec3, TBlockComponent> m_PredefinedBlocks;
};
