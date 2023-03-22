#include "MinecraftTerrainGenerator.h"
#include <nlohmann/json.hpp>
#include <unordered_map>
#include <spdlog/spdlog.h>

#include "BiomeGeneratorFactory.h"
#include "game/utils/FileUtils.h"
#include "game/utils/MathUtils.h"

static constexpr std::string_view BIOMES_CONFIG_PATH    = "res/configs/biomes.json";
static constexpr std::string_view GENERATOR_CONFIG_PATH = "res/configs/world_generation.json";

//
// Construction/Destruction
//

CMinecraftTerrainGenerator::CMinecraftTerrainGenerator(registry_t & Registry, int Seed) :
  m_Seed(Seed)
{
  m_RandomEngine.seed(Seed);

  auto && Map = QueryOrCreate<TTerrainMap>(Registry);

  m_ParametersMap = &Map;

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

  const float Height    = GetHeightAt(_Position);
  const int   Elevation = m_TerrainLevel / 2.f + (m_TerrainLevel / 2.f) * Height;

  if (static_cast<int>(_Position.y) > Elevation)
  {
    const auto Generator = GetBiomeGeneratorAt(_Position);

    return TBlockComponent{ .Type = Generator->GetBlockAt(Elevation, _Position.y, _Position.x, _Position.z) };
  }

  return TBlockComponent{ .Type = EBlockType::Stone };
}

float CMinecraftTerrainGenerator::GetHeightAt(glm::vec3 _Position) const
{
  if (const auto Iterator = m_ParametersMap->Blocks.find(glm::ivec2(_Position.x, _Position.z)); Iterator != m_ParametersMap->Blocks.cend())
    return Iterator->second.Height;

  const float ContinentalnessFactor = m_ContinentalnessMapNoise.GetNoise(_Position.x, _Position.z);
  const float ErosionFactor         = m_ErosionSplines[GetContinentalnessIndex(ContinentalnessFactor)].eval(To01(m_ErosionNoise.GetNoise(_Position.x, _Position.z))).result()[1];
  const float PeaksValleysFactor    = m_PeaksValleysSplines[GetErosionIndex(ErosionFactor)].eval(To01(m_PeaksValleysNoise.GetNoise(_Position.x, _Position.z))).result()[1];

  const float Height = ToNegative11(PeaksValleysFactor);

  m_ParametersMap->Blocks[glm::ivec2(_Position.x, _Position.z)] = TTerrainBlockInfo
  {
    .Continentalness = ContinentalnessFactor,
    .Erosion         = ErosionFactor,
    .PeaksValleys    = PeaksValleysFactor,
    .Height          = Height,
    .Temperature     = m_TemperatureNoise.GetNoise(_Position.x, _Position.z),
    .Humidity        = m_HumidityNoise.GetNoise(_Position.x, _Position.z)
  };

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

int CMinecraftTerrainGenerator::GetBiomeIDAt(glm::vec3 _Position)
{
  const TTerrainBlockInfo & Info = m_ParametersMap->Blocks.at(glm::ivec2(_Position.x, _Position.z));

  return 0;

  const int ContinentalnessIndex = Info.GetContinentalnessIndex();
  const int Erosion              = Info.GetErosionIndex();

  return m_RegularBiomeGenerationScheme.at(ContinentalnessIndex).at(Erosion);
}

CBiomeGenerator * CMinecraftTerrainGenerator::GetBiomeGeneratorAt(glm::vec3 _Position)
{
  if (const int BiomeID = GetBiomeIDAt(_Position); BiomeID != -1)
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

  int Seed = m_Seed;

  nlohmann::json Config = nlohmann::json::parse(Utils::ReadFile(GENERATOR_CONFIG_PATH));

  m_TerrainLevel         = Config["terrain_level"];
  m_WaterLevel           = Config["water_level"];
  m_LowerGenerationBound = Config["lower_generation_bound"];
  m_UpperGenerationBound = Config["upper_generation_bound"];

  m_ContinentalnessMapNoise.SetSeed(Seed++);
  m_ContinentalnessMapNoise.SetFrequency(Config["continentalness_noise"]["frequency"]);
  m_ContinentalnessMapNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);

  m_ErosionNoise.SetSeed(m_Seed++);
  m_ErosionNoise.SetFrequency(Config["erosion_noise"]["frequency"]);
  m_ErosionNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);

  m_PeaksValleysNoise.SetSeed(m_Seed++);
  m_PeaksValleysNoise.SetFrequency(Config["peaks_valleys_noise"]["frequency"]);
  m_PeaksValleysNoise.SetFractalType(FastNoiseLite::FractalType_Ridged);
  m_PeaksValleysNoise.SetFractalOctaves(Config["peaks_valleys_noise"]["fractal_octaves"]);
  m_PeaksValleysNoise.SetFractalGain(Config["peaks_valleys_noise"]["fractal_gain"]);
  m_PeaksValleysNoise.SetFractalLacunarity(Config["peaks_valleys_noise"]["fractal_lacunarity"]);
  m_PeaksValleysNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);

  m_TerrainShapeNoise.SetSeed(m_Seed++);
  m_TerrainShapeNoise.SetFrequency(Config["terrain_shape_noise"]["frequency"]);
  m_TerrainShapeNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);

  m_ContinentalnessSplineNoise.SetSeed(m_Seed++);
  m_TerrainShapeNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
  m_ContinentalnessSplineNoise.SetFrequency(Config["spline_noise"]["frequency"]);

  m_ErosionSplineNoise.SetSeed(m_Seed++);
  m_ErosionSplineNoise.SetNoiseType(FastNoiseLite::NoiseType_ValueCubic);
  m_ErosionSplineNoise.SetFrequency(Config["spline_noise"]["frequency"]);

  m_PeaksValleysSplineNoise.SetSeed(m_Seed++);
  m_PeaksValleysSplineNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
  m_PeaksValleysSplineNoise.SetFrequency(Config["spline_noise"]["frequency"]);

  m_TemperatureNoise.SetSeed(m_Seed++);
  m_TemperatureNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
  m_TemperatureNoise.SetFrequency(Config["temperature_noise"]["frequency"]);

  m_HumidityNoise.SetSeed(m_Seed++);
  m_HumidityNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
  m_HumidityNoise.SetFrequency(Config["humidity_noise"]["frequency"]);

  LoadSplines(Config);
  LoadBiomeGenerationSchemes(Config);
}

void CMinecraftTerrainGenerator::LoadBiomeGenerationSchemes(const nlohmann::json & Config)
{
  const auto RegularSchemeConfig = Config["biome_generation_schemes"]["regular"];

  for (int TerrainLevel = 0; TerrainLevel < RegularSchemeConfig.size(); TerrainLevel++)
  {
    const auto LevelBiomes = RegularSchemeConfig[TerrainLevel];

    for (int Humidity = 0; Humidity < LevelBiomes.size(); Humidity++)
      m_RegularBiomeGenerationScheme[TerrainLevel].push_back(LevelBiomes[Humidity]);
  }
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
      const int    SplineIndex  = To01(SplineNoise.GetNoise(Position.x + i, Position.z + j)) * 10;
      const auto & Spline       = Splines[SplineIndex];
      const float  Noise        = To01(Noise_.GetNoise(Position.x + i, Position.y, Position.z + j));

      Value += Spline.eval(Noise).result()[1];

      Count++;
    }
  }

  return Value /= Count;
}

int CMinecraftTerrainGenerator::GetContinentalnessIndex(float ContinentalnessValue) const
{
  static std::array ContinentalnessIndexes{ -1.2,-1.05,-0.455,-0.19,-0.11,0.03,0.3,0.8,1.0 };

  for (int i = 0; i < ContinentalnessIndexes.size(); i++)
  {
    if (ContinentalnessValue < ContinentalnessIndexes[i])
      return i;
  }

  return -1;
}

int CMinecraftTerrainGenerator::GetErosionIndex(float ErosinValue) const
{
  static std::array ErosionIndexes{ -1.0, -0.78, -0.375, -0.2225, 0.05, 0.45, 0.55, 1.0 };

  for (int i = 0; i < ErosionIndexes.size(); i++)
  {
    if (ErosinValue < ErosionIndexes[i])
      return i;
  }

  return -1;
}
