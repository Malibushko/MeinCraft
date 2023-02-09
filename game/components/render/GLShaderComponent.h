#pragma once
#include "core/entity/Component.h"

struct TGLShaderComponent
{
  static constexpr ComponentTag ComponentTag{};

  static constexpr unsigned int INVALID_SHADER_ID = -1;

  unsigned int ShaderID{};

  [[nodiscard]] bool IsValid() const
  {
    return ShaderID != INVALID_SHADER_ID && ShaderID != 0;
  }
};