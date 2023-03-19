#pragma once
#include <map>
#include <random>
#include <glm/gtx/hash.hpp>
#include <unordered_set>
#include <glm/vec3.hpp>
#include <nlohmann/json.hpp>
#include <tsl/robin_map.h>
#include "game/components/terrain/BlockComponent.h"
#include "third_party/FastNoise/FastNoiseLite.h"

#include "BiomeGenerator.h"
#include <tinysplinecxx.h>

class CMinecraftTerrainGenerator
{
public: // Construction/Destruction

  CMinecraftTerrainGenerator(int Seed = 0);

  ~CMinecraftTerrainGenerator() = default;

public: // Interface

  TBlockComponent Generate(glm::vec3 _Position);

protected: // Service

  float GetHeightAt(glm::vec3 _Position) const;

  std::vector<TBiomeConfig> LoadConfig(std::string_view _Path);

  int GetBiomeIDAt(glm::vec3 _Position);

  CBiomeGenerator * GetBiomeGeneratorAt(glm::vec3 _Position);

  void LoadConfigs();

  void LoadSplines(const nlohmann::json & Config);

  void LoadSplines(const nlohmann::json & Config, std::vector<tinyspline::BSpline> & Splines);

  void LoadSpline(const nlohmann::json & Config, tinyspline::BSpline & Spline);

  float GetNoiseValueAtSmoothed(
      const FastNoiseLite &                    Noise,
      const glm::vec3                          Position,
      const FastNoiseLite &                    SplineNoise,
      const std::vector<tinyspline::BSpline> & Splines
    ) const;

protected: // Members

  std::mt19937 m_RandomEngine;

  int                                             m_Seed{};
  std::map<int, std::unique_ptr<CBiomeGenerator>> m_BiomeGenerators;

  FastNoiseLite m_TerrainShapeNoise;

  // 2D noises to select spline for terrain region
  FastNoiseLite m_ContinentalnessSplineNoise;
  FastNoiseLite m_ErosionSplineNoise;
  FastNoiseLite m_PeaksValleysSplineNoise;

  // 2D noises to generate terrain
  FastNoiseLite m_ContinentalnessMapNoise;
  FastNoiseLite m_ErosionNoise;
  FastNoiseLite m_PeaksValleysNoise;

  // Splines
  std::vector<tinyspline::BSpline> m_ContinentalnessSplines;
  std::vector<tinyspline::BSpline> m_ErosionSplines;
  std::vector<tinyspline::BSpline> m_PeaksValleysSplines;

  // 2D noises for biome selection
  FastNoiseLite m_TemperatureNoise;
  FastNoiseLite m_PrecipitationNoise;

  int m_TerrainLevel{};
  int m_WaterLevel{};
  int m_LowerGenerationBound{};
  int m_UpperGenerationBound{};

  mutable tsl::robin_map<glm::ivec2, float> m_HeightMap;
};
