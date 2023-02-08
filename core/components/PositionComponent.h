#pragma once
#include "core/entity/Component.h"
#include <glm/vec3.hpp>

struct TPositionComponent : IComponent
{
  glm::vec3 Position;
};
