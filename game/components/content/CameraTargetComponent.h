#pragma once
#include <array>

#include "core/entity/Component.h"
#include "core/entity/Entity.h"

struct TCameraTargetComponent
{
  static constexpr ComponentTag ComponentTag{};

  entity_t  Target{ entt::null };
  glm::vec3 WorldPosition;
  bool      Refreshed = false;
};
