#pragma once
#include <cassert>
#include <chrono>
#include <functional>
#include <vector>
#include <magic_enum.hpp>

#include "core/entity/Entity.h"
#include "core/entity/Registry.h"
#include "core/entity/System.h"
#include "core/entity/FrameListenerSystem.h"

#include <boost/pfr/ops.hpp>
#include <spdlog/spdlog.h>

#include "core/entity/Component.h"

struct ISystem;

class World
{
public: // Public enums

  enum class EState
  {
    Setup, Running, Finishing, Finished, Destroyed
  };

public: // Construction/Destruction

  World()
  {
    SetState(EState::Setup);
  }

  ~World()
  {
    SetState(EState::Destroyed);
  }

public: // Interface

  World & AddStartupFunction(std::function<void(World &)> Startup_)
  {
    assert(m_State == EState::Setup && "Startup function will never be called!");

    m_StartupCallbacks.push_back(std::move(Startup_));

    return *this;
  }

  template<class T, class ... Args>
  World & AddSystem(Args && ... Args_)
  {
    m_Systems.emplace_back(std::make_unique<T>(std::forward<Args>(Args_)...));

    if constexpr (std::is_base_of_v<IFrameListenerSystem, T>)
      m_FrameListenerSystems.emplace_back(static_cast<IFrameListenerSystem*>(m_Systems.back().get()));

    return *this;
  }

  World & AddSystem(std::unique_ptr<ISystem> System_)
  {
    m_Systems.push_back(std::move(System_));

    return *this;
  }

  template<class T>
  World & Spawn(T && Component_)
  {
    ::Spawn(m_Registry, std::forward<T>(Component_));

    return *this;
  }

  template<class T>
  World & SpawnBundle(T && Bundle_)
  {
    ::SpawnBundle(m_Registry, std::forward<T>(Bundle_));

    return *this;
  }

  void Run()
  {
    for (auto & StartupCall : m_StartupCallbacks)
      StartupCall(*this);

    SetState(EState::Running);

    for (const auto & System : m_Systems)
      System->OnCreate(m_Registry);

    while (m_State == EState::Running)
    {
      auto LastTick = std::chrono::high_resolution_clock::now();

      for (const auto & System : m_FrameListenerSystems)
        System->OnFrameBegin(m_Registry);

      for (const auto & System : m_Systems)
      {
        const auto  Now   = std::chrono::high_resolution_clock::now();
        const float Delta = std::chrono::duration<float>(Now - LastTick).count();

        System->OnUpdate(m_Registry, Delta);

        LastTick = Now;
      }

      for (const auto & System : m_FrameListenerSystems)
        System->OnFrameEnd(m_Registry);
    }

    for (const auto & System : m_Systems)
      System->OnDestroy(m_Registry);

    SetState(EState::Finished);

    m_Systems.clear();
  }

  registry_t & Registry()
  {
    return m_Registry;
  }

  void RequestFinish()
  {
    SetState(EState::Finishing);
  }

private: // Service

  void SetState(EState State_)
  {
    m_State = State_;

    spdlog::info("World state changed to {}", magic_enum::enum_name(State_));
  }

private: // Members

  EState m_State{ EState::Setup };

  registry_t m_Registry;

  std::vector<std::unique_ptr<ISystem>>     m_Systems;
  std::vector<IFrameListenerSystem*>        m_FrameListenerSystems;
  std::vector<std::function<void(World &)>> m_StartupCallbacks;
};

