#pragma once
#include "core/entity/Component.h"

enum class EMouseButton
{
  Invalid = -1,
  Left,
  Right,
  Middle
};

enum class EMouseClickAction
{
  None = -1,
  Press,
  Release
};

struct TMouseClickData
{
  EMouseButton      Button{EMouseButton::Invalid};
  EMouseClickAction Action{EMouseClickAction::None};
};
