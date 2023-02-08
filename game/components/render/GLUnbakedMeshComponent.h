#pragma once
#include <vector>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

#include "core/entity/Component.h"

struct TGLUnbakedMeshComponent : IComponent
{
  std::vector<glm::vec3> Vertices;
  std::vector<int>       Indices;

  std::vector<glm::vec3> Normals;
  std::vector<glm::vec2> UV;
};
