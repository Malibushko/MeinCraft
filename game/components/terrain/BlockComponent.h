#pragma once
#include <vector>

#include "core/entity/Component.h"

enum class EBlockType
{
  Invalid         = -1,
  Air             = 0,
  Stone           = 1,
  Grass           = 2,
  Dirt            = 3,
  CobbleStone     = 4,
  StationaryWater = 9,
  StationaryLava  = 11,
  Sand            = 12,
  // TODO: support variations
  Wood            = 17,
  Leaves          = 18
};

struct TBlockComponent
{
  static constexpr ComponentTag ComponentTag{};

  EBlockType  Type { EBlockType::Invalid };

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
