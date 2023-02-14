#include "NoiseTerrainGenerator.h"

#include "game/utils/NumericUtils.h"

static constexpr float LOWER_GENERATION_BOUND = 0.f;
static constexpr float UPPER_GENERATION_BOUND = LOWER_GENERATION_BOUND + 65.f;
static constexpr float WATER_LEVEl            = 11.f;

//
// Construction/Destruction
//

CNoiseTerrainGenerator::CNoiseTerrainGenerator(int Seed)
{
  m_Noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
  m_Noise.SetSeed(Seed);
}

TBlockComponent CNoiseTerrainGenerator::Generate(glm::vec3 _Position)
{
  if (Utils::AlmostEqual(_Position.y, LOWER_GENERATION_BOUND) || _Position.y < LOWER_GENERATION_BOUND)
    return TBlockComponent{ .Type = EBlockType::Stone }; // TODO: replace with bedrock

  if (std::abs(_Position.y) >= UPPER_GENERATION_BOUND)
    return TBlockComponent{ .Type = EBlockType::Air };

  float Elevation = GetNoise(_Position.x, _Position.z) +
                    0.5f * GetNoise(2.f *  _Position.x + 1.2f,  2.f * _Position.z + 3.5f) +
                    0.25f * GetNoise(4.f * _Position.x + 17.1f, 4.f * _Position.z + 11.f);

  Elevation /= 1.f + 0.5f + 0.25f;
  Elevation = std::pow(Elevation, 1.2f);

  if (Elevation > _Position.y / UPPER_GENERATION_BOUND)
  {
    if (Elevation < 0.2f)
      return TBlockComponent{ .Type = EBlockType::Sand };

    if (Generate(glm::vec3(_Position.x, _Position.y + 1.f, _Position.z)).Type == EBlockType::Air)
      return TBlockComponent{ .Type = EBlockType::Grass };

    return TBlockComponent{ .Type = EBlockType::Dirt };
  }

  if (_Position.y <= WATER_LEVEl && Generate(glm::vec3(_Position.x, _Position.y - 1.f, _Position.z)).Type != EBlockType::Air)
    return TBlockComponent{ .Type = EBlockType::StationaryWater };

  return TBlockComponent{ .Type = EBlockType::Air };
}

float CNoiseTerrainGenerator::GetNoise(float x, float y)
{
  // Translate noise from [-1, 1] to [0, 1]
  return m_Noise.GetNoise(x, y) * 0.5f + 0.5f;
}
