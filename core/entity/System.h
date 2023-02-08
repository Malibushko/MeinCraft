#pragma once
#include "core/entity/Registry.h"

struct ISystem
{
   virtual void OnCreate(registry_t & Registry_) = 0;

   virtual void OnDestroy(registry_t & Registry_) = 0;

   virtual void OnUpdate(registry_t & Registry_, float Delta_) = 0;

   virtual ~ISystem() = default;
};
