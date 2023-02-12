#pragma once
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include "DirectedLightComponent.h"
#include "LightComponent.h"
#include "core/entity/Component.h"

struct TDirectedLightBundle
{
  static constexpr BundleTag BundleTag{};

  TLightComponent         Light;
  TDirectedLightComponent DirectedLight;
};
