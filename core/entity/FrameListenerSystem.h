#pragma once
#include "System.h"

struct IFrameListenerSystem : ISystem
{
   virtual void OnFrameBegin(registry_t & Registry_) = 0;

   virtual void OnFrameEnd(registry_t & Registry_) = 0;

   virtual ~IFrameListenerSystem() = default;
};
