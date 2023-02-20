#pragma once
#include <Ultralight/Renderer.h>
#include <Ultralight/View.h>
#include <Ultralight/platform/Logger.h>

#include "core/entity/System.h"

class CUltraLightUISystem : public ISystem
{
public: // Construction/Destruction

  CUltraLightUISystem();

  ~CUltraLightUISystem() override;

public: // ISystem

  void OnCreate(registry_t  & Registry_) override;

  void OnUpdate(registry_t  & Registry_, float Delta_) override;

  void OnDestroy(registry_t & Registry_) override;
};