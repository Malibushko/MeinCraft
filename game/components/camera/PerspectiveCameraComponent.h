#pragma once
#include "core/entity/Component.h"

struct TPerspectiveCameraComponent : IComponent
{
  float FOV{};
  float AspectRatio{};
  float Near{};
  float Far{};
};