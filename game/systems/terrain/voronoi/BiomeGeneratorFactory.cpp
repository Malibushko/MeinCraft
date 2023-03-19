#include "BiomeGeneratorFactory.h"

#include "biomes/DesertBiomeGenerator.h"
#include "biomes/MissingBlockBiomeGenerator.h"
#include "biomes/MountainBiomeGenerator.h"
#include "biomes/OceanBiomeGenerator.h"
#include "biomes/PlainsBiomeGenerator.h"

enum class EBiomeType : int {
  Ocean = 0,
  Plains = 1,
  Desert = 2,
  Mountains = 3,
  Forest = 4,
  Taiga = 5,
  Swamp = 6,
  River = 7,
  NetherWastes = 8,
  TheEnd = 9,
  FrozenOcean = 10,
  FrozenRiver = 11,
  SnowyTundra = 12,
  SnowyMountains = 13,
  MushroomFields = 14,
  MushroomFieldShore = 15,
  Beach = 16,
  DesertHills = 17,
  WoodedHills = 18,
  TaigaHills = 19,
  MountainEdge = 20,
  Jungle = 21,
  JungleHills = 22,
  JungleEdge = 23,
  DeepOcean = 24,
  StoneShore = 25,
  SnowyBeach = 26,
  BirchForest = 27,
  BirchForestHills = 28,
  DarkForest = 29,
  SnowyTaiga = 30,
  SnowyTaigaHills = 31,
  GiantTreeTaiga = 32,
  GiantTreeTaigaHills = 33,
  WoodedMountains = 34,
  Savanna = 35,
  SavannaPlateau = 36,
  Badlands = 37,
  WoodedBadlandsPlateau = 38,
  BadlandsPlateau = 39,
  TheVoid = 127,
  SunflowerPlains = 129,
  DesertLakes = 130,
  GravellyMountains = 131,
  FlowerForest = 132,
  TaigaMountains = 133,
  SwampHills = 134,
  IceSpikes = 140,
  ModifiedJungle = 149,
  ModifiedJungleEdge = 151,
  TallBirchForest = 155,
  TallBirchHills = 156,
  DarkForestHills = 157,
  SnowyTaigaMountains = 158,
  GiantSpruceTaiga = 160,
  GiantSpruceTaigaHills = 161,
  ModifiedGravellyMountains = 162,
  ShatteredSavanna = 163,
  ShatteredSavannaPlateau = 164,
  ErodedBadlands = 165,
  ModifiedWoodedBadlandsPlateau = 166,
  ModifiedBadlandsPlateau = 167
};

//
// Interface
//

std::unique_ptr<CBiomeGenerator> CBiomeGeneratorFactory::Create(const TBiomeConfig & Config)
{
  switch (static_cast<EBiomeType>(Config.ID))
  {
    case EBiomeType::Ocean:
      return std::make_unique<COceanBiomeGenerator>(Config);

    case EBiomeType::Plains:
      return std::make_unique<CPlainsBiomeGenerator>(Config);

    case EBiomeType::Desert:
      return std::make_unique<CDesertBiomeGenerator>(Config);

    case EBiomeType::Mountains:
      return std::make_unique<CMountainBiomeGenerator>(Config);
  }

  return std::make_unique<CMissingBlockBiomeGenerator>(Config);
}