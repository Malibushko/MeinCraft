#pragma once
#include <glad/glad.h>
#include "core/entity/System.h"
#include "game/components/render/GLShaderComponent.h"

class GLRenderScreenPassSystem : public ISystem
{
public: // ISystem

  void OnCreate(registry_t & Registry_) override;

  void OnUpdate(registry_t & Registry_, float Delta_) override;

  void OnDestroy(registry_t & Registry_) override;

protected: // Members

  TGLShaderComponent m_CompositeShader;
  TGLShaderComponent m_ScreenShader;

  GLuint             m_ScreenQuadVAO{};

#ifdef DEBUG_DEPTH

  TGLShaderComponent m_DebugDepthShader;

#endif
};
