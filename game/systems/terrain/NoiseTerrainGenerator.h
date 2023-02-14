#pragma once
#include <glm/vec3.hpp>

#include "game/components/terrain/BlockComponent.h"
#include "third_party/FastNoise/FastNoiseLite.h"

class CNoiseTerrainGenerator
{
public: // Construction/Destruction

  CNoiseTerrainGenerator(int Seed = 0);

  ~CNoiseTerrainGenerator() = default;

public: // Interface

  TBlockComponent Generate(glm::vec3 _Position);

protected: // Service

  float GetNoise(float x, float y);

private: // Members

  FastNoiseLite m_Noise;
};
