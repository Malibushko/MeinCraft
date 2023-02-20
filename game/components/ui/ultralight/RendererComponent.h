#pragma once
#include <Ultralight/RefPtr.h>
#include <Ultralight/Renderer.h>

struct TUltraLightRendererComponent
{
  static constexpr ComponentTag ComponentTag{};

  ultralight::RefPtr<ultralight::Renderer> Renderer;
};
