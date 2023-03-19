#include "TerrainGeneratorUtils.h"

int Utils::SquuezeTemperature(float Value)
{
  if (Value < 0.0f)
    return 0;

  if (Value < 0.1f)
    return 1;

  if (Value < 0.45f)
    return 2;

  if (Value < 0.55f)
    return 3;

  if (Value < 0.65f)
    return 4;

  if (Value < 0.8f)
    return 5;

  return 6;
}

int Utils::SqueezeHumidity(float Value)
{
  if (Value < 0.1f)
    return 0;

  if (Value < 0.4f)
    return 1;

  if (Value < 0.5f)
    return 2;

  if (Value < 0.7f)
    return 3;

  return 4;
}
