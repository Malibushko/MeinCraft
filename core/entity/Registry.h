#pragma once
#include <entt/entity/registry.hpp>
#include <boost/pfr/ops.hpp>

#include "core/entity/Entity.h"
#include "core/entity/Component.h"

using registry_t = entt::registry;

template<class T>
T & QueryFirst(registry_t & Registry_)
{
  assert(Registry_.view<T>().size() == 1 && "Query returned more than one result!");

  return Registry_.get<T>(Registry_.view<T>().front());
}

template<class T>
std::pair<entity_t, T &> QueryOrCreate(registry_t & Registry_)
{
  if (auto View = Registry_.view<T>(); View.size() >= 1)
    return { View.front(), Registry_.get<T>(View.front()) };

  auto Entity = Registry_.create();

  return { Entity, Registry_.emplace<T>(Entity) };
}

template<class T>
void AddComponent(registry_t & Registry_, entity_t Entity_, T && Component_)
{
  boost::pfr::for_each_field(Component_, [&](auto && Field_)
  {
    if constexpr (std::is_base_of_v<IComponent, std::decay_t<decltype(Field_)>>)
      AddComponent(Entity_, Field_);
  });

  Registry_.emplace<T>(Entity_, std::forward<T>(Component_));
}

template<class T>
void AddBundle(registry_t & Registry_ ,entity_t Entity_, T && Bundle_)
{
  boost::pfr::for_each_field(std::forward<T>(Bundle_), [&](auto && Field_)
  {
    static_assert(std::is_base_of_v<IComponent, std::decay_t<decltype(Field_)>>);

    Registry_.emplace<std::decay_t<decltype(Field_)>>(Entity_, decltype(Field_)(Field_));
  });
}

template<class T>
registry_t & Spawn(registry_t & Registry_, T && Component_)
{
  entity_t Entity = Registry_.create();

  Registry_.emplace<T>(Entity, std::forward<T>(Component_));

  return Registry_;
}

template<class T>
registry_t & SpawnBundle(registry_t & Registry_, T && Bundle_)
{
  entity_t Entity = Registry_.create();

  AddBundle(Registry_, Entity, std::forward<T>(Bundle_));

  return Registry_;
}