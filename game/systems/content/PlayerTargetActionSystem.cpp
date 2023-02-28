#include "PlayerTargetActionSystem.h"

#include <glm/ext/matrix_transform.hpp>

#include "core/components/PositionComponent.h"
#include "core/components/TransformComponent.h"
#include "game/components/camera/CameraBasisComponent.h"
#include "game/components/camera/GlobalTransformComponent.h"
#include "game/components/content/CameraTargetComponent.h"
#include "game/components/content/inventory/InventoryComponent.h"
#include "game/components/events/CameraChangedEvent.h"
#include "game/components/input/MouseClickData.h"
#include "game/components/requests/ItemUseRequest.h"
#include "game/components/requests/PlayerHitRequest.h"
#include "game/components/requests/RaycastRequest.h"
#include "game/components/terrain/BlockComponent.h"
#include "game/factory/BlockFactory.h"
#include "game/resources/ShaderLibrary.h"
#include "game/resources/TextureLibrary.h"

//
// Construction/Destruction
//

CPlayerTargetActionSystem::CPlayerTargetActionSystem() = default;

CPlayerTargetActionSystem::~CPlayerTargetActionSystem() = default;

//
// ISystem
//

void CPlayerTargetActionSystem::OnCreate(registry_t& Registry_)
{
  // Empty
}

void CPlayerTargetActionSystem::OnUpdate(registry_t & Registry_, float Delta_)
{
  for (auto [Entity, ClickData]: Registry_.view<TMouseClickData>().each())
  {
    if (ClickData.Action == EMouseClickAction::Press)
    {
      if (ClickData.Button == EMouseButton::Left)
      {
        if (const auto & Target = QuerySingle<TCameraTargetComponent>(Registry_); Target.Target != entt::null)
        {
          auto [_, HitData] = Create<TPlayerHitRequest>(Registry_);

          HitData.Target         = Target.Target;
          HitData.TargetPosition = Target.TargetWorldPosition;
        }
      }
      else
      {
        auto & Inventory = QuerySingle<TInventoryComponent>(Registry_);

        const entity_t SelectedEntity = Inventory.ItemsPanel[Inventory.SelectedItemIndex];

        if (SelectedEntity != entt::null)
        {
          auto [Entity, Request] = Create<TItemUseRequest>(Registry_);

          Request.Item = SelectedEntity;
        }
      }
    }
  }
}


void CPlayerTargetActionSystem::OnDestroy(registry_t& Registry_)
{
  // Empty
}
