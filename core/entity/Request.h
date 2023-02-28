#pragma once
#include <entt/entity/entity.hpp>

enum class ERequestStatus
{
  Created,
  Fulfilled,
  Rejected
};

struct TRequest
{
  void *         Owner{ nullptr };
  ERequestStatus Status{ERequestStatus::Created};

  bool           DestroyWhenFulfilled{ false };
  bool           DestroyWhenRejected{ false };
};

struct TOneshotRequest : TRequest
{
  TOneshotRequest()
  {
    DestroyWhenFulfilled = true;
    DestroyWhenRejected  = true;
  }
};