#pragma once
#include <entt/entity/entity.hpp>

struct TRequest
{
  void * Owner = nullptr;
  bool   IsFulfilled{false};
};