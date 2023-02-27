#pragma once
#include <entt/entity/registry.hpp>
#include <boost/pfr/ops.hpp>

#include "core/entity/Entity.h"
#include "core/entity/Component.h"

using registry_t = entt::basic_registry<entity_t>;

namespace details
{
  template<class T>
  concept is_bundle = requires {
    T::BundleTag;
  };

  template<class T>
  concept is_component = requires {
    T::ComponentTag;
  };
}

template<class T>
T & GetComponent(registry_t & Registry, entity_t Entity)
{
  return Registry.get<T>(Entity);
}

template<class ... T>
decltype(auto) GetComponents(registry_t & Registry, entity_t Entity)
{
  return Registry.get<T...>(Entity);
}

template<class ... T>
decltype(auto) QuerySingle(registry_t & Registry_)
{
  return Registry_.get<T...>(Registry_.view<T...>().front());
}

template<class T>
decltype(auto) QueryOrCreate(registry_t & Registry_)
{
  if (auto View = Registry_.view<T>(); View.size() >= 1)
    return Registry_.get<T>(View.front());

  const auto Entity = Registry_.create();

  return Registry_.emplace<T>(Entity);
}

template<class T>
std::pair<entity_t, T &> Create(registry_t & Registry_)
{
  auto Entity = Registry_.create();

  return { Entity, Registry_.emplace<T>(Entity) };
}

template<class T>
void AddComponent(registry_t & Registry_, entity_t Entity_, T && Component_)
{
  boost::pfr::for_each_field(Component_, [&](auto && Field_)
  {
    if constexpr (details::is_component<std::decay_t<decltype(Field_)>>)
      AddComponent(Entity_, Field_);
  });

  Registry_.emplace<T>(Entity_, std::forward<T>(Component_));
}

template<class T>
void AddBundle(registry_t & Registry_ ,entity_t Entity_, T && Bundle_)
{
  boost::pfr::for_each_field(std::forward<T>(Bundle_), [&](auto && Field_)
  {
    using field_t = std::decay_t<decltype(Field_)>;

    if constexpr (details::is_component<field_t>)
      Registry_.emplace<field_t>(Entity_, decltype(Field_)(Field_));
    else if constexpr(details::is_bundle<field_t>)
      AddBundle(Registry_, Entity_, decltype(Field_)(Field_));
    else
      static_assert([]{}, "Bundle must contain only components or other bundles!");
  });
}

template<class ... T>
registry_t & Spawn(registry_t & Registry_, T && ...Component_)
{
  entity_t Entity = Registry_.create();

  (Registry_.emplace<T>(Entity, std::forward<T>(Component_)), ...);

  return Registry_;
}

template<class T>
registry_t & SpawnBundle(registry_t & Registry_, T && Bundle_)
{
  entity_t Entity = Registry_.create();

  AddBundle(Registry_, Entity, std::forward<T>(Bundle_));

  return Registry_;
}

template<class T>
bool HasComponent(registry_t & Registry_, entity_t Entity_)
{
  return Registry_.try_get<T>(Entity_) != nullptr;
}

template<class T>
bool HasComponent(registry_t & Registry)
{
  return Registry.view<T>().size() > 0;
}