#include "FPSCounterSystem.h"

#include <spdlog/spdlog.h>

//
// Config
//

static constexpr float FPS_COUNTER_UPDATE_INTERVAL = 1000;

//
// Construction/Destruction
//

CFPSCounterSystem::CFPSCounterSystem() = default;

CFPSCounterSystem::~CFPSCounterSystem() = default;

//
// ISystem
//


void CFPSCounterSystem::OnCreate(registry_t & Registry_)
{
  // Empty
}

void CFPSCounterSystem::OnDestroy(registry_t & Registry_)
{
  // Empty
}

void CFPSCounterSystem::OnUpdate(registry_t & Registry_, float Delta_)
{
  m_DeltaAccumulator += Delta_;
  m_FramesCount++;

  if (m_DeltaAccumulator >= FPS_COUNTER_UPDATE_INTERVAL)
  {
    spdlog::info("FPS: {}, Entities: {}", static_cast<float>(m_FramesCount), Registry_.size());

    m_DeltaAccumulator = 0;
    m_FramesCount      = 0;
  }
}


