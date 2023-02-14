#pragma once
#include <vector>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

#include "GLMeshComponent.h"
#include "core/entity/Component.h"

template <EMeshType T>
struct TGLUnbakedMeshComponent
{
  static constexpr ComponentTag ComponentTag{};

  std::vector<glm::vec3>      Vertices;
  std::vector<unsigned short> Indices;

  std::vector<glm::vec3>      Normals;
  std::vector<glm::vec2>      UV;
};

using TGLUnbakedSolidMeshComponent       = TGLUnbakedMeshComponent<EMeshType::Solid>;
using TGLUnbakedTranslucentMeshComponent = TGLUnbakedMeshComponent<EMeshType::Translucent>;