#pragma once
#include <glad/glad.h>
#include <glm/vec3.hpp>

#include "core/entity/System.h"
#include "game/components/render/GLShaderComponent.h"

class GLRenderSSAOPassSystem : public ISystem
{
public: // ISystem

  void OnCreate(registry_t & Registry_) override;

  void OnUpdate(registry_t & Registry_, float Delta_) override;

  void OnDestroy(registry_t & Registry_) override;

protected: // Service

  GLuint m_FBO{};
  GLuint m_BlurFBO{};

  GLuint m_SSAOTexture{};
  GLuint m_SSAOBlurTexture{};

  GLuint m_NoiseTexture{};
  std::vector<glm::vec3> m_Kernel;

  GLuint             m_ScreenQuadVAO{};

  TGLShaderComponent m_SSAOShader{};
  TGLShaderComponent m_SSAOBlurShader{};
};
