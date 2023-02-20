#include "UltraLightLoggerSystem.h"

#include <Ultralight/Ultralight.h>
#include <Ultralight/platform/Logger.h>
#include <AppCore/Platform.h>
#include <spdlog/spdlog.h>

#include "game/components/display/DisplayComponent.h"

//
// Construction/Destruction
//

CUltraLightUILoggerSystem::CUltraLightUILoggerSystem() = default;

CUltraLightUILoggerSystem::~CUltraLightUILoggerSystem() = default;

//
// ISystem
//

void CUltraLightUILoggerSystem::OnCreate(registry_t & Registry_)
{
  ultralight::Platform::instance().set_logger(this);
}

void CUltraLightUILoggerSystem::OnUpdate(registry_t & Registry_, float Delta_)
{
  // Empty
}

void CUltraLightUILoggerSystem::OnDestroy(registry_t & Registry_)
{
  // Empty
}

//
// ultralight::Logger
//

void CUltraLightUILoggerSystem::LogMessage(ultralight::LogLevel LogLevel, const ultralight::String16 & Message)
{
  ultralight::String8 Message8 = ultralight::String(Message).utf8();

  switch (LogLevel)
  {
    case ultralight::kLogLevel_Error:
      spdlog::error("[UI]: {}", Message8.data());
      break;
    case ultralight::kLogLevel_Info:
      spdlog::info("[UI]: {}", Message8.data());
      break;
    case ultralight::kLogLevel_Warning:
      spdlog::warn("[UI]: {}", Message8.data());
      break;
  }
}
