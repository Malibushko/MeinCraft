#pragma once
#include "core/entity/Component.h"
#include <Ultralight/View.h>

struct TUltraLightViewComponent
{
  static constexpr ComponentTag ComponentTag{};

  ultralight::RefPtr<ultralight::View> View;
};
