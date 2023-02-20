#pragma once
#include <Ultralight/Renderer.h>
#include <Ultralight/View.h>

#include "UltraLightGLTextureSurface.h"
#include "core/entity/System.h"
#include "game/components/render/GLShaderComponent.h"

class CUltraLightUIRenderSystem : public ISystem
{
public: // Construction/Destruction

  CUltraLightUIRenderSystem();

  ~CUltraLightUIRenderSystem() override;

public: // ISystem

  void OnCreate(registry_t  & Registry_) override;

  void OnUpdate(registry_t  & Registry_, float Delta_) override;

  void OnDestroy(registry_t & Registry_) override;

protected: // Service

  void InitGLUtils();

protected: // Members

  TGLShaderComponent m_ScreenQuadShader{};
  GLuint m_ScreenVAO{};
  std::unique_ptr<UltraLightGLTextureSurfaceFactory> m_SurfaceFactory;
};
