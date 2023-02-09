#pragma once
#include "core/entity/Component.h"

struct TPerspectiveCameraComponent
{
  static constexpr ComponentTag ComponentTag{};

  float FOV{};
  float AspectRatio{};
  float Near{};
  float Far{};
};