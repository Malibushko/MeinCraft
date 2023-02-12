#pragma once
#include "DirectedLightBundle.h"
#include "core/components/TransformComponent.h"
#include "game/components/terrain/BlockComponent.h"

struct TWorldLightBundle
{
  static constexpr BundleTag BundleTag{};

  TTransformComponent  Transform;
  TDirectedLightBundle DirectedLight;
};
