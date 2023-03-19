#include "MinecraftTerrainGenerator.h"
#include <nlohmann/json.hpp>
#include <unordered_map>
#include <spdlog/spdlog.h>

#include "BiomeGeneratorFactory.h"
#include "game/utils/FileUtils.h"

static constexpr std::string_view BIOMES_CONFIG_PATH    = "res/configs/biomes.json";
static constexpr std::string_view GENERATOR_CONFIG_PATH = "res/configs/world_generation.json";

//
// Construction/Destruction
//

CMinecraftTerrainGenerator::CMinecraftTerrainGenerator(int Seed) :
  m_Seed(Seed)
{
  m_RandomEngine.seed(Seed);

  LoadConfigs();
}

//
// Interface
//

TBlockComponent CMinecraftTerrainGenerator::Generate(glm::vec3 _Position)
{
  if (static_cast<int>(_Position.y) <= m_LowerGenerationBound)
    return TBlockComponent{ .Type = EBlockType::Bedrock };

  if (static_cast<int>(_Position.y) >= m_UpperGenerationBound) // $FIXME: probably wrong for underworld but need revision later
    return TBlockComponent{ .Type = EBlockType::Air };

  const float Height = GetHeightAt(_Position) - (_Position.y - m_TerrainLevel) / m_TerrainLevel;

  if (Height < 0)
  {
    if (static_cast<int>(_Position.y) <= m_WaterLevel)
      return TBlockComponent{ .Type = EBlockType::StationaryWater };

    return TBlockComponent{ .Type = EBlockType::Air };
  }

  return TBlockComponent{ .Type = EBlockType::Stone };
}

float CMinecraftTerrainGenerator::GetHeightAt(glm::vec3 _Position) const
{
  if (const auto Iterator = m_HeightMap.find(glm::ivec2(_Position.x, _Position.z)); Iterator != m_HeightMap.cend())
    return Iterator->second;

  const float ContinentalnessFactor = GetNoiseValueAtSmoothed(m_ContinentalnessMapNoise, _Position, m_ContinentalnessSplineNoise, m_ContinentalnessSplines);
  const float ErosionFactor         = GetNoiseValueAtSmoothed(m_ErosionNoise, _Position, m_ErosionSplineNoise, m_ErosionSplines);
  const float PeaksValleysFactor    = GetNoiseValueAtSmoothed(m_PeaksValleysNoise, _Position, m_PeaksValleysSplineNoise, m_PeaksValleysSplines);

  const float Height = (ContinentalnessFactor - ErosionFactor) + (PeaksValleysFactor - 0.5) * 2;

  m_HeightMap[glm::ivec2(_Position.x, _Position.z)] = Height;

  return Height;
}

//
// Service
//

std::vector<TBiomeConfig> CMinecraftTerrainGenerator::LoadConfig(std::string_view _Path)
{
  using json = nlohmann::json;

  const json JsonData = json::parse(Utils::ReadFile(_Path));

  if (JsonData.is_null())
    return {};

  std::vector<TBiomeConfig> BiomeConfigs;

  for (const auto & Json : JsonData)
  {
    TBiomeConfig Config;

    Config.ID            = Json["id"];
    Config.Name          = Json["name"];
    Config.NameLegacy    = Json["name_legacy"];
    Config.Category      = Json["category"];
    Config.Temperature   = Json["temperature"];
    Config.Precipitation = Json["precipitation"];
    Config.Depth         = Json["depth"];
    Config.Dimension     = Json["dimension"];
    Config.DisplayName   = Json["displayName"];
    Config.Color         = Json["color"];
    Config.Rainfall      = Json["rainfall"];

    if (Json.find("climates") != Json.end())
    {
      for (const auto & ClimateJSON : Json["climates"])
      {
        TClimate Climate{};

        Climate.Temperature = ClimateJSON["temperature"];
        Climate.Humidity    = ClimateJSON["humidity"];
        Climate.Altitude    = ClimateJSON["altitude"];
        Climate.Weirdness   = ClimateJSON["weirdness"];
        Climate.Offset      = ClimateJSON["offset"];

        Config.Climates.push_back(Climate);
      }
    }

    BiomeConfigs.push_back(Config);
  }

  return BiomeConfigs;
}

int CMinecraftTerrainGenerator::GetBiomeIDAt(float Height, glm::vec3 _Position)
{
  const float Temperature   = m_TemperatureNoise.GetNoise(_Position.x, _Position.z);
  const float Precipitation = m_PrecipitationNoise.GetNoise(_Position.x, _Position.z);

  int   ID         = -1;
  float BestSuit   = 0.f;
  float BiomeDepth = 0.f;
  /*
  for (const int BiomeID : GetBiomeIDsForDepth(Height, std::numeric_limits<float>::epsilon()))
  {
    const auto & BiomeGenerator = m_BiomeGenerators.at(BiomeID);

    if (const float SuitFactor = BiomeGenerator->Suit(Temperature, Precipitation); SuitFactor > BestSuit)
    {
      ID         = BiomeGenerator->GetID();
      BestSuit   = SuitFactor;
    }
  }
  */
  return ID;
}

CBiomeGenerator * CMinecraftTerrainGenerator::GetBiomeGeneratorAt(float Height, glm::vec3 _Position)
{
  if (const int BiomeID = GetBiomeIDAt(Height, _Position); BiomeID != -1)
    return m_BiomeGenerators.at(BiomeID).get();

  return nullptr;
}

void CMinecraftTerrainGenerator::LoadConfigs()
{
  if (const auto Configs = LoadConfig(BIOMES_CONFIG_PATH); Configs.empty())
  {
    spdlog::error("Failed to load biome config");
  }
  else
  {
    for (const auto & BiomeConfig : Configs)
      m_BiomeGenerators[BiomeConfig.ID] = CBiomeGeneratorFactory::Create(BiomeConfig);
  }

  nlohmann::json Config = nlohmann::json::parse(Utils::ReadFile(GENERATOR_CONFIG_PATH));

  m_TerrainLevel         = Config["terrain_level"];
  m_WaterLevel           = Config["water_level"];
  m_LowerGenerationBound = Config["lower_generation_bound"];
  m_UpperGenerationBound = Config["upper_generation_bound"];

  m_ContinentalnessMapNoise.SetSeed(m_Seed);
  m_ContinentalnessMapNoise.SetFrequency(Config["continentalness_noise"]["frequency"]);
  m_ContinentalnessMapNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);

  m_ErosionNoise.SetSeed(m_Seed + 1);
  m_ErosionNoise.SetFrequency(Config["erosion_noise"]["frequency"]);
  m_ErosionNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);

  m_PeaksValleysNoise.SetSeed(m_Seed + 2);
  m_PeaksValleysNoise.SetFrequency(Config["peaks_valleys_noise"]["frequency"]);
  m_PeaksValleysNoise.SetFractalType(FastNoiseLite::FractalType_Ridged);
  m_PeaksValleysNoise.SetFractalOctaves(Config["peaks_valleys_noise"]["fractal_octaves"]);
  m_PeaksValleysNoise.SetFractalGain(Config["peaks_valleys_noise"]["fractal_gain"]);
  m_PeaksValleysNoise.SetFractalLacunarity(Config["peaks_valleys_noise"]["fractal_lacunarity"]);
  m_PeaksValleysNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);

  m_TerrainShapeNoise.SetSeed(m_Seed + 3);
  m_TerrainShapeNoise.SetFrequency(Config["terrain_shape_noise"]["frequency"]);
  m_TerrainShapeNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);

  m_ContinentalnessSplineNoise.SetSeed(m_Seed + 4);
  m_ContinentalnessSplineNoise.SetFrequency(Config["spline_noise"]["frequency"]);

  m_ErosionSplineNoise.SetSeed(m_Seed + 5);
  m_ErosionSplineNoise.SetFrequency(Config["spline_noise"]["frequency"]);

  m_PeaksValleysSplineNoise.SetSeed(m_Seed + 6);
  m_PeaksValleysSplineNoise.SetFrequency(Config["spline_noise"]["frequency"]);

  LoadSplines(Config);
}

void CMinecraftTerrainGenerator::LoadSplines(const nlohmann::json & Config)
{
  LoadSplines(Config["continentalness_noise"]["curvature_splines"], m_ContinentalnessSplines);
  LoadSplines(Config["erosion_noise"]["curvature_splines"],         m_ErosionSplines);
  LoadSplines(Config["peaks_valleys_noise"]["curvature_splines"],   m_PeaksValleysSplines);
}

void CMinecraftTerrainGenerator::LoadSplines(const nlohmann::json & Config, std::vector<tinyspline::BSpline> & Splines)
{
  Splines.resize(Config.size());

  int SplineIndex = 0;

  for (const auto & SplineConfig : Config)
    LoadSpline(SplineConfig, Splines[SplineIndex++]);
}

void CMinecraftTerrainGenerator::LoadSpline(const nlohmann::json & Config, tinyspline::BSpline & Spline)
{
  std::vector<tinyspline::real> Points;

  for (const auto & Point : Config)
  {
    Points.push_back(Point["X"]);
    Points.push_back(Point["Y"]);
  }

  Spline = tinyspline::BSpline(Points.size() / 2);
  Spline.setControlPoints(Points);
}

float CMinecraftTerrainGenerator::GetNoiseValueAtSmoothed(
    const FastNoiseLite &                    Noise_,
    const glm::vec3                          Position,
    const FastNoiseLite &                    SplineNoise,
    const std::vector<tinyspline::BSpline> & Splines
  ) const
{
  constexpr int NeighbourCount = 8;

  float Value = 0.f;

  int Count = 0.0;

  for (int i = -NeighbourCount/ 2; i < NeighbourCount / 2; i++)
  {
    for (int j = -NeighbourCount / 2; j < NeighbourCount / 2; j++)
    {
      const int    SplineIndex = (SplineNoise.GetNoise(Position.x + i, Position.z + j) * 0.5f + 0.5f) * 10;
      const auto & Spline       = Splines[SplineIndex];
      const float  Noise        = Noise_.GetNoise(Position.x + i, Position.y, Position.z + j) * 0.5f + 0.5f;

      Value += Spline.eval(Noise).result()[1];

      Count++;
    }
  }

  return Value /= Count;
}
