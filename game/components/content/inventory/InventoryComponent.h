#pragma once
#include <array>

#include "core/entity/Component.h"
#include "core/entity/Entity.h"

struct TInventoryComponent
{
  static constexpr ComponentTag ComponentTag{};

  static constexpr size_t MAX_INVENTORY_SIZE   = 128;
  static constexpr size_t MAX_ITEMS_PANEL_SIZE = 9;

  std::array<entity_t, MAX_INVENTORY_SIZE>   Inventory;
  std::array<entity_t, MAX_ITEMS_PANEL_SIZE> ItemsPanel;
  size_t                                     SelectedItemIndex{};
};
