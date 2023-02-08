#pragma once
#include "CameraBundle.h"
#include "PerspectiveCameraComponent.h"

#include "core/entity/Bundle.h"

struct TPerspectiveCameraBundle
{
  static constexpr BundleTag BundleTag{};

  TCameraBundle               Camera{};
  TPerspectiveCameraComponent Perspective{};
};
