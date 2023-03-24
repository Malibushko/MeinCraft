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

  unsigned int                MaterialID = 0;

  [[nodiscard]] std::pair<glm::vec3, glm::vec3> GetBounds() const
  {
    if (Vertices.empty())
      return {};

    glm::vec3 Min = Vertices[0];
    glm::vec3 Max = Vertices[0];

    for (const auto & Vertex : Vertices)
    {
      if (Vertex.x < Min.x) Min.x = Vertex.x;
      if (Vertex.y < Min.y) Min.y = Vertex.y;
      if (Vertex.z < Min.z) Min.z = Vertex.z;
      if (Vertex.x > Max.x) Max.x = Vertex.x;
      if (Vertex.y > Max.y) Max.y = Vertex.y;
      if (Vertex.z > Max.z) Max.z = Vertex.z;
    }

    return { Min, Max };
  }
};

using TGLUnbakedSolidMeshComponent       = TGLUnbakedMeshComponent<EMeshType::Solid>;
using TGLUnbakedTranslucentMeshComponent = TGLUnbakedMeshComponent<EMeshType::Translucent>;