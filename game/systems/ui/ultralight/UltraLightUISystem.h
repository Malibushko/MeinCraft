#pragma once
#include <AppCore/Window.h>
#include <Ultralight/Renderer.h>
#include <Ultralight/View.h>
#include <Ultralight/platform/Logger.h>
#include "core/entity/System.h"

class CUltraLightUISystem : public ISystem, ultralight::LoadListener
{
public: // Construction/Destruction

  CUltraLightUISystem();

  ~CUltraLightUISystem() override;

public: // ISystem

  void OnCreate(registry_t  & Registry_) override;

  void OnUpdate(registry_t  & Registry_, float Delta_) override;

  void OnDestroy(registry_t & Registry_) override;

protected: // ultralight::WindowListener

  void OnDOMReady(ultralight::View * Caller, uint64_t FrameId, bool IsMainFrame, const ultralight::String & Url) override;

protected: // Service

  void UpdateDebugControls(registry_t & Registry);
};