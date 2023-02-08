#pragma once
#include "CameraComponent.h"

struct TPerspectiveCameraComponent
{
  TCameraComponent Camera{};

  float FOV{};
  float AspectRatio{};
  float Near{};
  float Far{};
};