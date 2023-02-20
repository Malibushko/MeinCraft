#include "NoiseTerrainGenerator.h"

#include <unordered_map>


#include "game/components/terrain/ChunkComponent.h"
#include "game/utils/NumericUtils.h"

static constexpr float LOWER_GENERATION_BOUND = 0.f;
static constexpr float UPPER_GENERATION_BOUND = LOWER_GENERATION_BOUND + 65.f;
static constexpr float WATER_LEVEl            = 11.f;
static constexpr float MINIMUM_TREE_HEIGHT    = 4.f;
static constexpr int   TREE_HEIGHT_DISPERSION = 8.f;

static constexpr int MINIMUM_TREE_CROWN_WIDTH  = 4;
static constexpr int MINIMUM_TREE_CROWN_HEIGHT = 2;
static constexpr int MINIMUM_TREE_CROWN_DEPTH  = 4;

static const std::unordered_map<glm::vec3, EBlockType> TREE_MESH =
{
  {{0, 0, 0}, EBlockType::Wood},
  {{0, 1, 0}, EBlockType::Wood},
  {{0, 2, 0}, EBlockType::Wood},
  {{0, 3, 0}, EBlockType::Wood},
  {{0, 4, 0}, EBlockType::Wood},
};

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
  if (const auto Iterator = m_PredefinedBlocks.find(_Position); Iterator != m_PredefinedBlocks.end())
    return Iterator->second;

  return GenerateImpl(_Position);
}

TBlockComponent CNoiseTerrainGenerator::GenerateImpl(glm::vec3 _Position)
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
    {

      if (m_Noise.GetNoise(_Position.x * 50.f, _Position.z * 50.f) > 0.85f && _Position.y > 15)
      {
        SpawnTreeAt(_Position);

        return Generate(_Position);
      }

      return TBlockComponent{ .Type = EBlockType::Grass };
    }

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

void CNoiseTerrainGenerator::SpawnTreeAt(glm::vec3 _Position)
{
  const int TreeSeed = rand();

  glm::vec3 TreeTopPosition;

  // Spawn tree stem
  for (int i = 0; i < MINIMUM_TREE_HEIGHT + TreeSeed % TREE_HEIGHT_DISPERSION; i++)
  {
    TreeTopPosition = _Position + glm::vec3(0, i, 0);

    m_PredefinedBlocks.emplace(TreeTopPosition, TBlockComponent{ EBlockType::Wood });
  }

  // Spawn crown
  const int CrownWidth  = MINIMUM_TREE_CROWN_WIDTH  + (TreeSeed % 4);
  const int CrownHeight = MINIMUM_TREE_CROWN_HEIGHT + (TreeSeed % 2);
  const int CrownDepth  = MINIMUM_TREE_CROWN_DEPTH  + (TreeSeed % 4);

  for (int X = 0; X < CrownWidth; X++)
  {
    for (int Y = 0; Y < CrownHeight; Y++)
    {
      for (int Z = 0; Z < CrownDepth; Z++)
      {
        glm::vec3 Offset = glm::vec3(X - std::floor(CrownWidth / 2.f), Y - std::floor(CrownHeight / 2.f), Z - std::floor(CrownDepth / 2.f));

        m_PredefinedBlocks.emplace(TreeTopPosition + Offset, TBlockComponent{ .Type = EBlockType::Leaves });
      }
    }
  }
}