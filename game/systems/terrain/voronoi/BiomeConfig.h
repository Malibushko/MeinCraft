#pragma once
#include <string>

#include "game/components/terrain/BlockComponent.h"

enum class EBiomeSteepness
{
  Valley,
  Low,
  Mid,
  High,
  Peak
};

enum class EBiomeContinentalness
{
  MushroomFields,
  DeepOcean,
  Ocean,
  Coast,
  NearInland,
  MidInland,
  FarInland
};

struct TClimate
{
  double Temperature;
  double Humidity;
  double Altitude;
  double Weirdness;
  double Offset;
};

struct TBiomeConfig
{
  int                   ID;
  std::string           Name;
  std::string           NameLegacy;
  std::string           Category;
  double                Temperature;
  std::string           Precipitation;
  double                Depth;
  std::string           Dimension;
  std::string           DisplayName;
  int                   Color;
  double                Rainfall;
  std::vector<TClimate> Climates;
};