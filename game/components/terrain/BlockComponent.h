#pragma once
#include <vector>

#include "core/entity/Component.h"

enum class EBlockType
{
  Invalid         = -1,
  Air             = 0,
  Stone           = 1,
  GrassBlock      = 2,
  Dirt            = 3,
  CobbleStone     = 4,
  Bedrock         = 7,
  StationaryWater = 9,
  StationaryLava  = 11,
  Sand            = 12,
  // TODO: support variations
  Wood            = 17,
  Leaves          = 18,
  Grass           = 31,
  YellowFlower    = 37,
  RedFlower       = 38,
  GlowStone       = 89,

  // Technical blocks
  Missing         = 1000
};

struct TBlockComponent
{
  static constexpr ComponentTag ComponentTag{};

  EBlockType  Type { EBlockType::Invalid };

public: // Interface

  [[nodiscard]] bool IsVisible() const
  {
    switch (Type)
    {
      case EBlockType::Invalid:
      case EBlockType::Air:
        return false;
    }

    return true;
  }
};
