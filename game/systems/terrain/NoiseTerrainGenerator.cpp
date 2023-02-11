#include "NoiseTerrainGenerator.h"

#include "game/utils/NumericUtils.h"

//
// Construction/Destruction
//

CNoiseTerrainGenerator::CNoiseTerrainGenerator()
{
  m_Noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
}

TBlockComponent CNoiseTerrainGenerator::Generate(const glm::vec3 & _Position)
{
  const float Value = m_Noise.GetNoise(_Position.x, _Position.y, _Position.z);

  if (Utils::AlmostEqual(_Position.y, 0.f))
  {
    return TBlockComponent
    {
      .Type = EBlockType::GrassDirt
    };
  }

  if (Value > 0.1f)
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
