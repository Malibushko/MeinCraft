#include "NumericUtils.h"

#include <array>
#include <glm/detail/func_geometric.inl>

float Utils::DistanceTo(const glm::vec3 _Position, const TBoundingVolumeComponent & Volume)
{
  float Distance = 0.f;

  std::visit([&](auto && Volume_)
  {
    Distance = DistanceTo(_Position, Volume_);
  }, Volume.Volume);

  return Distance;
}

float Utils::DistanceTo(const glm::vec3 _Position, const TAABBVolumeComponent & Volume)
{
  const std::array<glm::vec3, 8> Points =
  {
    glm::vec3(Volume.Min.x, Volume.Min.y, Volume.Min.z),
    glm::vec3(Volume.Max.x, Volume.Min.y, Volume.Min.z),
    glm::vec3(Volume.Min.x, Volume.Max.y, Volume.Min.z),
    glm::vec3(Volume.Max.x, Volume.Max.y, Volume.Min.z),
    glm::vec3(Volume.Min.x, Volume.Min.y, Volume.Max.z),
    glm::vec3(Volume.Max.x, Volume.Min.y, Volume.Max.z),
    glm::vec3(Volume.Min.x, Volume.Max.y, Volume.Max.z),
    glm::vec3(Volume.Max.x, Volume.Max.y, Volume.Max.z)
  };

  float MinDistance = std::numeric_limits<float>::max();

  for (auto & Point : Points)
    MinDistance = std::min(glm::length(_Position - Point), MinDistance);

  return MinDistance;
}

float Utils::DistanceTo(...)
{
  assert(false);
  return 0.f;
}
