#pragma once
#include "BiomeConfig.h"
#include "game/components/terrain/BlockComponent.h"

class CBiomeGenerator
{
public: // Interface

  CBiomeGenerator(TBiomeConfig Config) : m_Config(std::move(Config))
  {
    // Empty
  }

  virtual ~CBiomeGenerator()
  {
    // Empty
  }

  float Suit(const float Temperature, const float Precipitation) const
  {
    return m_Config.Rainfall - Precipitation + m_Config.Temperature - Temperature;
  }

  float GetDepth() const
  {
    return m_Config.Depth;
  }

  int GetID() const
  {
    return m_Config.ID;
  }

  virtual EBlockType GetBlockAt(int Elevation, int X_, int Y_) = 0;

protected: // Members

  TBiomeConfig m_Config;
};
