#include "NoesisUISystem.h"

#include <glad/glad.h>
#include <spdlog/spdlog.h>

#include "game/components/display/DisplayComponent.h"
#include <NsApp/LocalXamlProvider.h>
#include <NsApp/LocalTextureProvider.h>
#include <NsApp/LocalFontProvider.h>

//
// Construction/Destruction
//

CNoesisUISystem::CNoesisUISystem() = default;

CNoesisUISystem::~CNoesisUISystem() = default;

//
// Interface
//

void CNoesisUISystem::OnCreate(registry_t & Registry_)
{
  Noesis::SetLogHandler([](const char *, uint32_t, uint32_t Level, const char *, const char * Message)
  {
      if (Level == 0)
        spdlog::trace("[NOESIS]: {}", Message);
      if (Level == 1)
        spdlog::debug("[NOESIS]: {}", Message);
      if (Level == 2)
        spdlog::info("[NOESIS]: {}", Message);
      if (Level == 3)
        spdlog::info("[NOESIS]: {}", Message);
      if (Level == 4)
        spdlog::error("[NOESIS]: {}", Message);
  });

  // i dont care about keys
  Noesis::GUI::SetLicense(NS_LICENSE_NAME, NS_LICENSE_KEY);

  Noesis::GUI::Init();

  Noesis::GUI::SetXamlProvider(Noesis::MakePtr<NoesisApp::LocalXamlProvider>("."));
  Noesis::GUI::SetFontProvider(Noesis::MakePtr<NoesisApp::LocalFontProvider>("."));
  Noesis::GUI::SetTextureProvider(Noesis::MakePtr<NoesisApp::LocalTextureProvider>("."));
}

void CNoesisUISystem::OnUpdate(registry_t & Registry_, float Delta_)
{
}

void CNoesisUISystem::OnDestroy(registry_t & Registry_)
{
}




