#pragma once
#include "core/components/PositionComponent.h"

#include "CameraBasisComponent.h"
#include "GlobalTransformComponent.h"

struct TCameraBundle
{
  static constexpr BundleTag BundleTag{};

  TGlobalTransformComponent Transform{};
  TPositionComponent        Position{};
  TCameraBasisComponent     Basis{};
};
