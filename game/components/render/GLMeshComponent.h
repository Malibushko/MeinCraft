#pragma once
#include <glad/glad.h>

#include "core/entity/Component.h"
#include <glm/vec3.hpp>

enum class EMeshType
{
  Solid,
  Translucent
};

template<EMeshType T>
struct TGLMeshComponent
{
  static constexpr ComponentTag ComponentTag{};

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

using TGLSolidMeshComponent       = TGLMeshComponent<EMeshType::Solid>;
using TGLTranslucentMeshComponent = TGLMeshComponent<EMeshType::Translucent>;