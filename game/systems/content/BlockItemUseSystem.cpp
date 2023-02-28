#include "BlockItemUseSystem.h"

#include <glm/ext/matrix_transform.hpp>

#include "game/components/content/CameraTargetComponent.h"
#include "game/components/content/inventory/InventoryComponent.h"
#include "game/components/content/inventory/ItemComponent.h"
#include "game/components/input/MouseClickData.h"
#include "game/components/requests/BlockCreateRequest.h"
#include "game/components/requests/ItemUseRequest.h"
#include "game/components/requests/PlayerHitRequest.h"
#include "game/factory/BlockFactory.h"
#include "game/resources/ShaderLibrary.h"
#include "game/resources/TextureLibrary.h"
#include "game/utils/MathUtils.h"

//
// Construction/Destruction
//

CBlockItemUseSystem::CBlockItemUseSystem() = default;

CBlockItemUseSystem::~CBlockItemUseSystem() = default;

//
// ISystem
//

void CBlockItemUseSystem::OnCreate(registry_t& Registry_)
{
  // Empty
}

void CBlockItemUseSystem::OnUpdate(registry_t & Registry_, float Delta_)
{
  ProcessSentRequests(Registry_);

  for (auto [Entity, ItemRequest] : Registry_.view<TItemUseRequest>().each())
  {
    if (ItemRequest.Status != ERequestStatus::Created)
      continue;

    if (const auto BlockComponent = Registry_.try_get<TBlockComponent>(ItemRequest.Item); BlockComponent)
    {
      TCameraTargetComponent & Target = QuerySingle<TCameraTargetComponent>(Registry_);

      if (const TBlockComponent * TargetBlock = Registry_.try_get<TBlockComponent>(Target.Target); !TargetBlock)
      {
        ItemRequest.Status = ERequestStatus::Rejected;
      }
      else
      {
        const EBlockFace TargetBlockFace = GetBlockFaceIntersectingRay(Target.OriginPosition, glm::floor(Target.TargetWorldPosition));

        glm::vec3 TargetBlockPosition = glm::floor(Target.TargetWorldPosition);

        TargetBlockPosition += GetFaceDirection(TargetBlockFace);

        auto [Entity, Request] = Create<TBlockCreateRequest>(Registry_);

        Request.Owner         = this;
        Request.Type          = BlockComponent->Type;
        Request.BlockItem     = ItemRequest.Item;
        Request.WorldPosition = TargetBlockPosition;

        ItemRequest.Status = ERequestStatus::Fulfilled;
      }
    }
  }
}


void CBlockItemUseSystem::OnDestroy(registry_t& Registry_)
{
  // Empty
}

//
// Service
//

void CBlockItemUseSystem::ProcessSentRequests(registry_t & Registry)
{
  for (auto [Entity, Request] : Registry.view<TBlockCreateRequest>().each())
  {
    if (Request.Owner != this)
      continue;

    if (Request.Status == ERequestStatus::Fulfilled)
      Registry.get<TItemComponent>(Request.BlockItem).Count--;
  }
}
