#pragma once
#include "core/entity/Component.h"
#include <variant>
#include "AABBVolumeComponent.h"

struct TBoundingVolumeComponent
{
  std::variant<std::monostate, TAABBVolumeComponent> Volume;
};