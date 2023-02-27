#pragma once
#include <entt/entity/entity.hpp>

struct TRequest
{
  void * Owner = nullptr;
  bool   IsFulfilled{false};

  bool   DestroyWhenFulfilled{ false };
};

struct TOneshotRequest : TRequest
{
  TOneshotRequest()
  {
    DestroyWhenFulfilled = true;
  }
};