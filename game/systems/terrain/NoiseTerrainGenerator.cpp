#include "NoiseTerrainGenerator.h"

#include "game/utils/NumericUtils.h"

static constexpr float LOWER_GENERATION_BOUND = 0.f;
static constexpr float UPPER_GENERATION_BOUND = 65.f;

//
// Construction/Destruction
//

CNoiseTerrainGenerator::CNoiseTerrainGenerator()
{
  m_Noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
}

TBlockComponent CNoiseTerrainGenerator::Generate(const glm::vec3 & _Position)
{
  if (_Position.y <= LOWER_GENERATION_BOUND)
  {
    return TBlockComponent
    {
      .Type = EBlockType::GrassDirt
    };
  }
  else
  {
    if (_Position.y >= UPPER_GENERATION_BOUND)
    {
      return TBlockComponent{ .Type = EBlockType::Air };
    }
  }


  float Value = m_Noise.GetNoise(_Position.x, _Position.z);

  Value /= _Position.y * 0.17;

  if (Value >= 0.25f)
  {
    return TBlockComponent
    {
      .Type = EBlockType::GrassDirt
    };
  }
  else
  {
    return TBlockComponent
    {
      .Type = EBlockType::Air
    };
  }
}
