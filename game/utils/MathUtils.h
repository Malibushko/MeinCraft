#pragma once
#include <cmath>
#include <limits>
#include <iomanip>
#include <iostream>
#include <type_traits>
#include <algorithm>
#include <vector>
#include <glm/vec3.hpp>

#include "game/components/terrain/VisibleBlockFacesComponent.h"


EBlockFace GetBlockFaceIntersectingRay(glm::vec3 RayOrigin, glm::vec3 BlockPosition);

glm::vec3 GetFaceDirection(EBlockFace Face);

double TrigonometricInterpolation(const std::vector<std::pair<float, float>> & _Values);
