#include "UltraLightUISystem.h"

#include <Ultralight/Ultralight.h>
#include <Ultralight/platform/Logger.h>
#include <AppCore/Platform.h>
#include <spdlog/spdlog.h>
#include <AppCore/App.h>
#include <AppCore/Window.h>
#include <AppCore/Overlay.h>
#include <AppCore/JSHelpers.h>

#include "core/components/PositionComponent.h"
#include "game/components/camera/GlobalTransformComponent.h"
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

  View.View->set_load_listener(this);
  View.View->LoadURL("file:///res/ui/hud.html");
  View.View->Focus();
}

void CUltraLightUISystem::OnUpdate(registry_t & Registry_, float Delta_)
{
  UpdateDebugControls(Registry_);
}

void CUltraLightUISystem::OnDestroy(registry_t & Registry_)
{
  // Empty
}

void CUltraLightUISystem::OnDOMReady(
    ultralight::View *         Caller,
    uint64_t                   FrameId,
    bool                       IsMainFrame,
    const ultralight::String & Url
  )
{
  LoadListener::OnDOMReady(Caller, FrameId, IsMainFrame, Url);

  auto Context = Caller->LockJSContext();

  ultralight::SetJSContext(Context->ctx());
}

//
// Service
//

void CUltraLightUISystem::UpdateDebugControls(registry_t & Registry)
{
  auto [Transform, Position] = QuerySingle<TGlobalTransformComponent, TPositionComponent>(Registry);

  ultralight::JSObject Global = ultralight::JSGlobalObject();

  const std::string PositionString = std::format("Position: X: {:.2f}, Y: {:.2f}, Z: {:.2f}",
                                                  Position.Position.x, Position.Position.y, Position.Position.z);

  Global["position_text"].ToObject()["innerHTML"] = { PositionString.c_str() };
}
