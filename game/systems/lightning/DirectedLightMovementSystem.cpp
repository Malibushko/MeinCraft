#include "DirectedLightMovementSystem.h"

#include <glm/common.hpp>

#include "game/components/lightning/DirectedLightComponent.h"
#include <glm/detail/func_trigonometric.inl>

#include "game/utils/NumericUtils.h"

static constexpr float FULL_CYCLE_INTERVAL = 60 * 1000;
static constexpr float AFTER_CYCLE_PAUSE   = 10 * 1000;

static constexpr float MIN_INTENSITY       = 0.35f;
static constexpr float MAX_INTENSITY       = 1.f;

static constexpr glm::vec3 START_POINT     = glm::vec3(0.f, -35.f, 0.f);
static constexpr glm::vec3 PEAK_POINT      = glm::vec3(100.f, -100.f, 100.f);
static constexpr glm::vec3 FINAL_POINT     = glm::vec3(0.f, -35.f, 0.f);

//
// Construction/Destruction
//

CDirectedLightMovementSystem::CDirectedLightMovementSystem() = default;

CDirectedLightMovementSystem::~CDirectedLightMovementSystem() = default;

//
// ISystem
//

void CDirectedLightMovementSystem::OnCreate(registry_t & Registry_)
{
  m_MovementAccumulator = std::rand() % static_cast<int>(FULL_CYCLE_INTERVAL);
}

void CDirectedLightMovementSystem::OnUpdate(registry_t & Registry_, float Delta_)
{
  if (m_MovementAccumulator > FULL_CYCLE_INTERVAL)
  {
    m_MovementAccumulator = 0;
    m_PauseAccumulator    = AFTER_CYCLE_PAUSE;
  }

  if (m_PauseAccumulator >= 0.f)
  {
    m_PauseAccumulator -= Delta_;

    return;
  }

  auto & DirectedLight = QuerySingle<TDirectedLightComponent>(Registry_);

  if (m_MovementAccumulator >= FULL_CYCLE_INTERVAL / 2)
  {
    const float Progress = (m_MovementAccumulator - FULL_CYCLE_INTERVAL / 2) / (FULL_CYCLE_INTERVAL / 2);

    DirectedLight.Direction = Utils::Lerp(PEAK_POINT, FINAL_POINT,      Progress);
    DirectedLight.Intensity = Utils::Lerp(MAX_INTENSITY, MIN_INTENSITY, Progress);
  }
  else
  {
    const float Progress = m_MovementAccumulator / (FULL_CYCLE_INTERVAL / 2);

    DirectedLight.Direction = Utils::Lerp(START_POINT, PEAK_POINT,      Progress);
    DirectedLight.Intensity = Utils::Lerp(MIN_INTENSITY, MAX_INTENSITY, Progress);
  }

  m_MovementAccumulator += Delta_;
}

void CDirectedLightMovementSystem::OnDestroy(registry_t & Registry_)
{
  // Empty
}
