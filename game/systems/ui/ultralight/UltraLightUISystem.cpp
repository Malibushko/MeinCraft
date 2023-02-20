#include "UltraLightUISystem.h"

#include <Ultralight/Ultralight.h>
#include <Ultralight/platform/Logger.h>
#include <AppCore/Platform.h>
#include <spdlog/spdlog.h>

#include "game/components/display/DisplayComponent.h"
#include "game/components/ui/ultralight/RendererComponent.h"
#include "game/components/ui/ultralight/ViewComponent.h"
#include "game/utils/FileUtils.h"

//
// Construction/Destruction
//

CUltraLightUISystem::CUltraLightUISystem() = default;

CUltraLightUISystem::~CUltraLightUISystem() = default;

//
// ISystem
//

void CUltraLightUISystem::OnCreate(registry_t & Registry_)
{
  const auto & View = QuerySingle<TUltraLightViewComponent>(Registry_);

  View.View->LoadURL("file:///res/ui/hud.html");
  View.View->Focus();
}

void CUltraLightUISystem::OnUpdate(registry_t & Registry_, float Delta_)
{
  // Empty
}

void CUltraLightUISystem::OnDestroy(registry_t & Registry_)
{
  // Empty
}
