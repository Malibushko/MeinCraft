#pragma once
#include <NsRender/GLFactory.h>

#include "core/entity/Component.h"
#include "game/math/Frustum.h"

struct TGLRenderPassData
{
  static constexpr ComponentTag ComponentTag{};

  CFrustum RenderFrustum;

  GLuint MatricesBuffer{ 0 };
  GLuint CameraBuffer{ 0 };
  GLuint DirectedLightBuffer{ 0 };
  GLuint PointLightsBuffer{ 0 };
  GLuint PointLightIndicesBuffer{ 0 };
  GLuint MaterialsBuffer{ 0 };
  GLuint TerrainBuffer{ 0 };

  GLuint TerrainTexture3D{ 0 };

  // Forward+ data
  GLuint LightCullingWorkGroupsX = 0;
  GLuint LightCullingWorkGroupsY = 0;

  // Texture that will be rendered in the final pass
  GLuint ScreenTexture{};

  // Posteffects
  GLuint SSAOTexture{};

  // General purpose textures
  GLuint DepthTexture{};
  GLuint NormalTexture{};
  GLuint PositionsTexture{};

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
