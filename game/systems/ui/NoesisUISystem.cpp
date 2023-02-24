#include "NoesisUISystem.h"

#include <glad/glad.h>
#include <spdlog/spdlog.h>

#include "game/components/display/DisplayComponent.h"

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
  TDisplayComponent & Display = QuerySingle<TDisplayComponent>(Registry_);

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

  Noesis::Ptr<Noesis::Grid> xaml(Noesis::GUI::ParseXaml<Noesis::Grid>(R"(
        <Grid xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation">
            <Grid.Background>
                <LinearGradientBrush StartPoint="0,0" EndPoint="0,1">
                    <GradientStop Offset="0" Color="#FF123F61"/>
                    <GradientStop Offset="0.6" Color="#FF0E4B79"/>
                    <GradientStop Offset="0.7" Color="#FF106097"/>
                </LinearGradientBrush>
            </Grid.Background>
            <Viewbox>
                <StackPanel Margin="50">
                    <Button Content="Hello World!" Margin="0,30,0,0"/>
                    <Rectangle Height="5" Margin="-10,20,-10,0">
                        <Rectangle.Fill>
                            <RadialGradientBrush>
                                <GradientStop Offset="0" Color="#40000000"/>
                                <GradientStop Offset="1" Color="#00000000"/>
                            </RadialGradientBrush>
                        </Rectangle.Fill>
                    </Rectangle>
                </StackPanel>
            </Viewbox>
        </Grid>
    )"));

  m_View = Noesis::GUI::CreateView(xaml);
  m_View->SetFlags(Noesis::RenderFlags_PPAA | Noesis::RenderFlags_LCD);
  m_View->SetSize(Display.Width, Display.Height);

  m_View->GetRenderer()->Init(NoesisApp::GLFactory::CreateDevice(false));
}

void CNoesisUISystem::OnUpdate(registry_t & Registry_, float Delta_)
{
  m_View->Update(Delta_);

  // Offscreen rendering phase populates textures needed by the on-screen rendering
  m_View->GetRenderer()->UpdateRenderTree();
  m_View->GetRenderer()->RenderOffscreen();

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  glEnable(GL_BLEND);

  m_View->GetRenderer()->Render();

  glDisable(GL_BLEND);
}

void CNoesisUISystem::OnDestroy(registry_t & Registry_)
{
  (void)m_View->Release();
}




