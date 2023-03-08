#pragma once
#include <NsRender/GLFactory.h>

#include "core/entity/Component.h"
#include "game/math/Frustum.h"

struct TGLRenderPassData
{
  static constexpr ComponentTag ComponentTag{};

  CFrustum RenderFrustum;

  GLuint   MatricesUBO{ 0 };
  GLuint   LightUBO{ 0 };
  GLuint   CameraUBO{ 0 };

  // Deferred shading data
  GLuint PositionTexture{};
  GLuint AlbedoTexture{};
  GLuint NormalTexture{};
  GLuint DepthTexture{};

  // Miscellaneous data
  GLuint SolidTexture{};
  GLuint TransparentTexture{};

  GLuint SolidFBO{};
  GLuint TransparentFBO{};

  // IOT stuff
  GLuint AccumulatorTexture{};
  GLuint RevealTexture{};
  GLuint CompositeDepthTexture{};
};
