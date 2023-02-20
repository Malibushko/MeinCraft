#pragma once
#include <Ultralight/Renderer.h>
#include <Ultralight/View.h>
#include <Ultralight/platform/Logger.h>

#include "core/entity/System.h"

class CUltraLightUILoggerSystem : public ISystem, ultralight::Logger
{
public: // Construction/Destruction

  CUltraLightUILoggerSystem();

  ~CUltraLightUILoggerSystem() override;

public: // ISystem

  void OnCreate(registry_t  & Registry_) override;

  void OnUpdate(registry_t  & Registry_, float Delta_) override;

  void OnDestroy(registry_t & Registry_) override;

protected: // ultralight::Logger

  void LogMessage(ultralight::LogLevel LogLevel, const ultralight::String16 & Message) override;
};