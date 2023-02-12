#pragma once
#include <vector>

#include "core/entity/Component.h"

enum class EBlockType
{
  Invalid = -1,
  Sun,
  Moon,
  Air,
  GrassDirt
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
