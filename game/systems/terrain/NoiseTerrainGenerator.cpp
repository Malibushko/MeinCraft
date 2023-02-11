#include "NoiseTerrainGenerator.h"

#include "game/utils/NumericUtils.h"

//
// Construction/Destruction
//

CNoiseTerrainGenerator::CNoiseTerrainGenerator()
{
  m_Noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
  m_Noise.SetFrequency(0.00522);
}

TBlockComponent CNoiseTerrainGenerator::Generate(const glm::vec3 & _Position)
{
  if (Utils::AlmostEqual(_Position.y, 0.f))
  {
    return TBlockComponent
    {
      .Type = EBlockType::GrassDirt
    };
  }

  float Value = m_Noise.GetNoise(_Position.x, _Position.y, _Position.z) +
                      0.5 * m_Noise.GetNoise(2 * _Position.x, 2 * _Position.y, 2 * _Position.z) +
                      0.25 * m_Noise.GetNoise(4 * _Position.x, 4 * _Position.y, 4 * _Position.z);

  Value /= 1 + 0.5 + 0.25;

  if (Value <= 0)
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
