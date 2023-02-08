#pragma once

struct TDisplayComponent
{
  size_t Width;
  size_t Height;

  float GetAspectRatio() const
  {
    return static_cast<float>(Width) / static_cast<float>(Height);
  }
};