#include "RequestsSystem.h"

#include "core/entity/Request.h"

void CRequestsSystem::OnCreate(registry_t & Registry_)
{
  // Empty
}

void CRequestsSystem::OnUpdate(registry_t & Registry_, float Delta_)
{
  for (auto [Entity, Request] : Registry_.view<TRequest>().each())
  {
    if (Request.DestroyWhenFulfilled && Request.Status == ERequestStatus::Fulfilled)
      Registry_.destroy(Entity);

    if (Request.DestroyWhenRejected && Request.Status == ERequestStatus::Rejected)
      Registry_.destroy(Entity);
  }
}

void CRequestsSystem::OnDestroy(registry_t & Registry_)
{
  // Empty
}
