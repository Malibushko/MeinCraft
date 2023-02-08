#pragma once
#include "core/entity/Component.h"

struct TGLMeshComponent : IComponent
{
  static constexpr unsigned int INVALID_ID = -1;

  unsigned int VAO           = INVALID_ID;
  unsigned int VBO           = INVALID_ID;
  unsigned int EBO           = INVALID_ID;
  unsigned int IndicesCount  = 0;

  [[nodiscard]] bool IsBaked() const
  {
    return VAO != INVALID_ID && VBO != INVALID_ID && EBO != INVALID_ID;
  }
};
