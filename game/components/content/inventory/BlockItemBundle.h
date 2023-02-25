#pragma once
#include <array>

#include "BlockItemBundle.h"
#include "ItemComponent.h"
#include "core/entity/Component.h"
#include "core/entity/Entity.h"
#include "game/components/terrain/BlockComponent.h"

struct TBlockItemBundle
{
  static constexpr BundleTag BundleTag{};

  TItemComponent  ItemData;
  TBlockComponent Block;
};
