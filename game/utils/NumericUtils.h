#pragma once
#include <cmath>
#include <limits>
#include <iomanip>
#include <iostream>
#include <type_traits>
#include <algorithm>

namespace Utils
{

template<class T>
typename std::enable_if<!std::numeric_limits<T>::is_integer, bool>::type
AlmostEqual(T x, T y, int ulp = 2)
{
  // the machine epsilon has to be scaled to the magnitude of the values used
  // and multiplied by the desired precision in ULPs (units in the last place)
  return std::fabs(x - y) <= std::numeric_limits<T>::epsilon() * std::fabs(x + y) * ulp
    // unless the result is subnormal
    || std::fabs(x - y) < std::numeric_limits<T>::min();
}

}
