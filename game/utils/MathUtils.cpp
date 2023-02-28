#include "NumericUtils.h"

#include <array>
#include <glm/detail/func_geometric.inl>

#include "MathUtils.h"

EBlockFace GetBlockFaceIntersectingRay(glm::vec3 RayOrigin, glm::vec3 BlockPosition)
{
  static constexpr std::array<glm::vec3, 6> BlockFaceCenters
  {
    // Front
    glm::vec3(0.5f, 0.f, 0.f),
    // Back
    glm::vec3(-0.5f, 0.f, 0.f),
    // Left
    glm::vec3(0.f, 0.f, 0.5f),
    // Right
    glm::vec3(0.f, 0.f, -0.5f),
    // Top
    glm::vec3(0.f, 0.5f, 0.f),
    // Bottom
    glm::vec3(0.0, -0.5f, 0.f)
  };

  size_t MinFaceIndex = 0;
  float  MinDistance  = std::numeric_limits<float>::max();

  for (size_t i = 0; i < BlockFaceCenters.size(); i++)
  {
    if (const float Distance = glm::distance(BlockPosition + BlockFaceCenters[i], RayOrigin); Distance < MinDistance)
    {
      MinFaceIndex = i;
      MinDistance  = Distance;
    }
  }

  return static_cast<EBlockFace>(1 << MinFaceIndex);
}

glm::vec3 GetFaceDirection(EBlockFace Face)
{
  switch (Face)
  {
    case EBlockFace::Front:
      return glm::vec3(1, 0, 0);
    case EBlockFace::Back:
      return glm::vec3(-1, 0, 0);
    case EBlockFace::Left:
      return glm::vec3(0, 0, 1);
    case EBlockFace::Right:
      return glm::vec3(0, 0, -1);
    case EBlockFace::Top:
      return glm::vec3(0, 1, 0);
    case EBlockFace::Bottom:
      return glm::vec3(0, -1, 0);
  }

  assert(false);
  return {};
}
