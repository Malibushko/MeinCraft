#include "NoesisUIHUDSystem.h"

#include <glad/glad.h>
#include <spdlog/spdlog.h>

#include "HUDItemsPanelItem.h"
#include "core/components/PositionComponent.h"
#include "game/components/camera/GlobalTransformComponent.h"
#include "game/components/content/CameraTargetComponent.h"
#include "game/components/content/inventory/InventoryComponent.h"
#include "game/components/content/inventory/ItemComponent.h"
#include "game/components/display/DisplayComponent.h"
#include "game/components/input/KeyboardState.h"
#include "game/components/input/MouseWheelData.h"
#include "game/components/terrain/BlockComponent.h"
#include "game/factory/BlockFactory.h"

static constexpr int ITEMS_PANEL_ITEMS_COUNT = 9;

//
// Construction/Destruction
//

CNoesisUIHUDSystem::CNoesisUIHUDSystem() = default;

CNoesisUIHUDSystem::~CNoesisUIHUDSystem() = default;

//
// Interface
//

void CNoesisUIHUDSystem::OnCreate(registry_t & Registry_)
{
  TDisplayComponent & Display = QuerySingle<TDisplayComponent>(Registry_);

  m_View = Noesis::GUI::CreateView(Noesis::GUI::LoadXaml<Noesis::Grid>("res/ui/HUD.xaml"));
  m_View->SetFlags(Noesis::RenderFlags_PPAA | Noesis::RenderFlags_LCD);
  m_View->SetSize(static_cast<uint32_t>(Display.Width), static_cast<uint32_t>(Display.Height));

  m_View->GetRenderer()->Init(NoesisApp::GLFactory::CreateDevice(true));

  m_DataModel = Noesis::MakePtr<CHUDDataModel>();

  InitItemsPanel(Registry_);

  m_View->GetContent()->SetDataContext(m_DataModel);
}

void CNoesisUIHUDSystem::OnUpdate(registry_t & Registry_, float Delta_)
{
  ProcessInput(Registry_);
  UpdateDebugPanel(Registry_);

  m_View->Update(Delta_);

  m_View->GetRenderer()->UpdateRenderTree();
  m_View->GetRenderer()->RenderOffscreen();

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  glEnable(GL_BLEND);

  m_View->GetRenderer()->Render();

  glDisable(GL_BLEND);
}

void CNoesisUIHUDSystem::OnDestroy(registry_t & Registry_)
{
  // Empty
}

void CNoesisUIHUDSystem::InitItemsPanel(registry_t & Registry)
{
  TInventoryComponent & Inventory = QuerySingle<TInventoryComponent>(Registry);

  for (int i = 0; i < ITEMS_PANEL_ITEMS_COUNT; i++)
  {
    const auto     ItemData   = Noesis::MakePtr<THUDItemsPanelItem>();
    const entity_t ItemEntity = Inventory.ItemsPanel[i];

    if (ItemEntity != entt::null)
    {
      TItemComponent & Item = Registry.get<TItemComponent>(ItemEntity);

      ItemData->SetCount(Item.Count);
      ItemData->SetItemEntity(ItemEntity);

      // TODO: refactor by moving icon management to a separate system
      if (TBlockComponent * BlockComponent = Registry.try_get<TBlockComponent>(ItemEntity))
        ItemData->SetIconPath(CBlockFactory::GetIconPath(*BlockComponent).data());
    }

    m_DataModel->AddItemsPanelItem(ItemData);
  }

  m_DataModel->SetActiveItemsPanelItem(Inventory.SelectedItemIndex);
}

void CNoesisUIHUDSystem::ProcessInput(registry_t & Registry)
{
  for (auto && [Entity, Wheel] : Registry.view<TMouseWheelData>().each())
  {
    const size_t ActiveIndex = m_DataModel->GetActiveItemsPanelItemIndex();

    if (Wheel.DeltaY > 0)
      m_DataModel->SetActiveItemsPanelItem((ActiveIndex - 1) % ITEMS_PANEL_ITEMS_COUNT);
    else
      m_DataModel->SetActiveItemsPanelItem((ActiveIndex + 1) % ITEMS_PANEL_ITEMS_COUNT);
  }

  const auto & Keyboard = QuerySingle<TKeyboardState>(Registry);

  const EKeyboardKey ActiveKey = static_cast<EKeyboardKey>(int(EKeyboardKey::KEY_0) + m_DataModel->GetActiveItemsPanelItemIndex());

  if (Keyboard[ActiveKey] != EKeyState::Pressed)
  {
    for (int Key = static_cast<int>(EKeyboardKey::KEY_0); Key != static_cast<int>(EKeyboardKey::KEY_9); Key++)
    {
      if (Keyboard[Key] == EKeyState::Pressed)
      {
        m_DataModel->SetActiveItemsPanelItem(Key - static_cast<int>(EKeyboardKey::KEY_0) - 1);
        break;
      }
    }
  }

  QuerySingle<TInventoryComponent>(Registry).SelectedItemIndex = m_DataModel->GetActiveItemsPanelItemIndex();
}

void CNoesisUIHUDSystem::UpdateDebugPanel(registry_t & Registry)
{
  auto [Transform, Position] = QuerySingle<TGlobalTransformComponent, TPositionComponent>(Registry);
  auto & Target              = QuerySingle<TCameraTargetComponent>(Registry);

  m_DataModel->SetDebugPosition(Position.Position);
  m_DataModel->SetDebugTargetPosition(Target.TargetWorldPosition);
}