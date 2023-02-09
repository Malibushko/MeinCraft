#pragma once
#include <vector>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

#include "core/entity/Component.h"

struct TGLTextureComponent
{
  static constexpr ComponentTag ComponentTag{};

  static constexpr unsigned int INVALID_SHADER_ID = -1;

  unsigned int TextureID = static_cast<unsigned int>(-1);

  [[nodiscard]] bool IsValid() const
  {
    return TextureID != INVALID_SHADER_ID;
  }
};
