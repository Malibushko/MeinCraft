#pragma once
#include "CameraBundle.h"
#include "PerspectiveCameraComponent.h"

struct TPerspectiveCameraBundle
{
  static constexpr BundleTag BundleTag{};

  TCameraBundle               Camera{};
  TPerspectiveCameraComponent Perspective{};
};