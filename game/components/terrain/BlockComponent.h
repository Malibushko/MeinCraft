#pragma once
#include <vector>

#include "core/entity/Component.h"

enum class EBlockType : uint8_t
{
  Invalid         = 255,
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
  Missing         = 254
};

enum class EBlockFlags : uint8_t
{
  None        = 0x0,
  LightSource = 0x1
};

struct TBlockComponent
{
  static constexpr ComponentTag ComponentTag{};

  EBlockType  Type { EBlockType::Invalid };
  EBlockFlags Flags{ EBlockFlags::None };

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