#pragma once
#include "core/entity/Component.h"

enum class EBlockFace
{
  None   = 0,
  Front  = 1 << 0,
  Back   = 1 << 1,
  Left   = 1 << 2,
  Right  = 1 << 3,
  Top    = 1 << 4,
  Bottom = 1 << 5,
  All    = Front | Back | Left | Right | Top | Bottom
};

inline EBlockFace operator |(EBlockFace Left, EBlockFace Right)
{
  return static_cast<EBlockFace>(static_cast<int>(Left) | static_cast<int>(Right));
}

inline EBlockFace operator &(EBlockFace Left, EBlockFace Right)
{
  return static_cast<EBlockFace>(static_cast<int>(Left) & static_cast<int>(Right));
}

inline int operator &(EBlockFace Left, int Flags)
{
  return static_cast<int>(Left) & Flags;
}

struct TVisibleBlockFacesComponent
{
  static constexpr ComponentTag ComponentTag{};

  EBlockFace Faces;
};

