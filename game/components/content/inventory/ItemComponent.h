#pragma once
#include <array>

#include "core/entity/Component.h"
#include "core/entity/Entity.h"

struct TItemComponent
{
  static constexpr ComponentTag ComponentTag{};

  int       Count{ 0 };
  int       StackSize{64};
};
