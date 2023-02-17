#pragma once
#include <glm/vec3.hpp>
#include <array>

#include "game/components/camera/GlobalTransformComponent.h"
#include "game/components/physics/BoundingVolume.h"

class CFrustum
{
  enum EPlane
  {
    Right, Left, Bottom, Top, Front, Back
  };

  enum
  {
    A, B, C, D
  };

public: // Construction/Destruction

  CFrustum();

  [[nodiscard]] static CFrustum FromTransform(const TGlobalTransformComponent & Transform);

public: // Interface

  [[nodiscard]] bool Intersect(const TBoundingVolumeComponent & Volume) const;

  [[nodiscard]] bool Intersect(const TAABBVolumeComponent & Volume) const;

  [[nodiscard]] bool Intersect(...) const;

  [[nodiscard]] size_t GetViewDistance() const;

  [[nodiscard]] glm::vec3 GetPosition() const;

protected: // Service

  void Normalize(EPlane Plane);

private: // Members

  std::array<glm::vec4, 6> m_Planes;
};
