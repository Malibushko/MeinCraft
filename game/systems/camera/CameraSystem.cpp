#include "CameraSystem.h"

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "game/components/camera/CameraBundle.h"
#include "game/components/camera/PerspectiveCameraComponent.h"
#include "game/components/input/KeyboardState.h"
#include "game/components/input/MouseMovementData.h"
#include "game/components/input/MouseWheelData.h"

#include "game/components/events/CameraChangedEvent.h"
#include "game/components/terrain/BlockComponent.h"

//
// Config
//

static constexpr int CAMERA_SPEED    = 50;
static constexpr int MOUSE_SENSIVITY = 10;

//
// Construction/Destruction
//

CCameraSystem::CCameraSystem() = default;

CCameraSystem::~CCameraSystem() = default;

//
// ISystem
//

void CCameraSystem::OnCreate(registry_t & Registry_)
{
  UpdateCameraTransforms(Registry_);
}

void CCameraSystem::OnUpdate(registry_t & Registry_, float Delta_)
{
  Delta_ /= 1000.f;

  auto & KeyboardState = QuerySingle<TKeyboardState>(Registry_);
  auto   CameraViews   = Registry_.view<TGlobalTransformComponent, TPositionComponent, TCameraBasisComponent>().each();

  bool IsNeedUpdateTransforms = false;

  Registry_.clear<TCameraChangedEvent>();

  for (const auto & [Entity, GlobalTransform, Position, Basis] : CameraViews)
  {
    if (KeyboardState[EKeyboardKey::KEY_W] == EKeyState::Pressed)
    {
      Position.Position     += Basis.Front * (CAMERA_SPEED * Delta_);
      IsNeedUpdateTransforms = true;
    }

    if (KeyboardState[EKeyboardKey::KEY_S] == EKeyState::Pressed)
    {
      Position.Position      -= Basis.Front * (CAMERA_SPEED * Delta_);;
      IsNeedUpdateTransforms = true;
    }

    if (KeyboardState[EKeyboardKey::KEY_A] == EKeyState::Pressed)
    {
      Position.Position      -= Basis.Right * (CAMERA_SPEED * Delta_);;
      IsNeedUpdateTransforms = true;
    }

    if (KeyboardState[EKeyboardKey::KEY_D] == EKeyState::Pressed)
    {
      Position.Position      += Basis.Right * (CAMERA_SPEED * Delta_);;
      IsNeedUpdateTransforms = true;
    }

    if (KeyboardState[EKeyboardKey::KEY_SPACE] == EKeyState::Pressed)
    {
      Position.Position      += Basis.Up * (CAMERA_SPEED * Delta_);;
      IsNeedUpdateTransforms = true;
    }

    for (const auto & [Entity, MouseMovement] : Registry_.view<TMouseMovementData>().each())
    {
      Basis.Yaw   += MouseMovement.DeltaX * (Delta_ * MOUSE_SENSIVITY);
      Basis.Pitch += MouseMovement.DeltaY * (Delta_ * MOUSE_SENSIVITY);

      Basis.Pitch = glm::clamp(Basis.Pitch, -89.f, 89.f);
      Basis.Yaw   = glm::mod(Basis.Yaw, 360.f);

      Basis.Front.x = cos(glm::radians(Basis.Yaw)) * cos(glm::radians(Basis.Pitch));
      Basis.Front.y = sin(glm::radians(Basis.Pitch));
      Basis.Front.z = sin(glm::radians(Basis.Yaw)) * cos(glm::radians(Basis.Pitch));
      Basis.Front   = glm::normalize(Basis.Front);

      Basis.Right = glm::normalize(glm::cross(Basis.Front, glm::vec3(0.f, 1.f, 0.f)));
      Basis.Up    = glm::normalize(glm::cross(Basis.Right, Basis.Front));

      IsNeedUpdateTransforms = true;
    }

    for (const auto & [Entity, Scroll] : Registry_.view<TMouseWheelData>().each())
    {
      if (auto Perspective = Registry_.try_get<TPerspectiveCameraComponent>(Entity);
          Perspective != nullptr)
      {
        Perspective->FOV -= Scroll.DeltaY * Delta_;
        Perspective->FOV = glm::clamp(Perspective->FOV, 1.f, 45.f);
      }
    }

    if (IsNeedUpdateTransforms)
    {
      UpdateCameraTransforms(Registry_);

      Create<TCameraChangedEvent>(Registry_);
    }
  }
}

void CCameraSystem::OnDestroy(registry_t& Registry_)
{
  // Empty
}

void CCameraSystem::UpdateCameraTransforms(registry_t & Registry_)
{
  auto CameraViews = Registry_.view<TGlobalTransformComponent, TPositionComponent, TCameraBasisComponent>().each();

  for (const auto & [Entity, GlobalTransform, Position, Basis] : CameraViews)
  {
    GlobalTransform.Model      = glm::mat4(1.f);
    GlobalTransform.View       = glm::lookAt(Position.Position, Position.Position + Basis.Front, Basis.Up);

    if (const auto Perspective = Registry_.try_get<TPerspectiveCameraComponent>(Entity);
        Perspective != nullptr)
    {
      GlobalTransform.Projection = glm::perspective(
          glm::radians(Perspective->FOV),
          Perspective->AspectRatio,
          Perspective->Near,
          Perspective->Far
        );
    }
    else
    {
      GlobalTransform.Projection = glm::mat4(1.f);
    }
  }
}




