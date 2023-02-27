#include "TargetControllerSystem.h"

#include <glm/ext/matrix_transform.hpp>

#include "core/components/PositionComponent.h"
#include "core/components/TransformComponent.h"
#include "game/components/camera/CameraBasisComponent.h"
#include "game/components/camera/GlobalTransformComponent.h"
#include "game/components/content/CameraTargetComponent.h"
#include "game/components/events/CameraChangedEvent.h"
#include "game/components/requests/RaycastRequest.h"
#include "game/components/terrain/BlockComponent.h"
#include "game/factory/BlockFactory.h"
#include "game/resources/ShaderLibrary.h"

static constexpr float MAX_TARGET_DISTANCE = 10;

//
// Construction/Destruction
//

CTargetControllerSystem::CTargetControllerSystem() = default;

CTargetControllerSystem::~CTargetControllerSystem() = default;

//
// ISystem
//

void CTargetControllerSystem::OnCreate(registry_t& Registry_)
{
  Create<TCameraTargetComponent>(Registry_);
}

void CTargetControllerSystem::OnUpdate(registry_t & Registry_, float Delta_)
{
  m_NeedToUpdateTarget = HasComponent<TCameraChangedEvent>(Registry_);

  auto & Target = QuerySingle<TCameraTargetComponent>(Registry_);

  Target.Refreshed = false;

  if (!m_NeedToUpdateTarget)
    return;

  for (const auto & [Entity, Request] : Registry_.view<TRaycastRequest>().each())
  {
    if (Request.Owner == this && Request.IsFulfilled)
    {
      Target.Target        = Request.RaycastHit;
      Target.WorldPosition = Request.EndPosition;
      Target.Refreshed     = true;

      Registry_.erase<TRaycastRequest>(Entity);

      m_NeedToUpdateTarget = false;

      break;
    }
  }

  if (m_NeedToUpdateTarget)
  {
    const auto & [Transform, Position, Basis] = QuerySingle<TGlobalTransformComponent, TPositionComponent, TCameraBasisComponent>(Registry_);

    TRaycastRequest Request;

    Request.Distance = MAX_TARGET_DISTANCE;
    Request.Owner    = this;

    Spawn<TRaycastRequest>(Registry_, std::move(Request));
  }
}


void CTargetControllerSystem::OnDestroy(registry_t& Registry_)
{
  // Empty
}

