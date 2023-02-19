#pragma once
#include <glad/glad.h>

#include "core/entity/System.h"
#include "game/components/camera/CameraBasisComponent.h"
#include "game/components/render/GLShaderComponent.h"
#include "game/math/Frustum.h"

struct TGLShaderComponent;

class GLRenderSystem : public ISystem
{
public: // Construction/Destruction

  GLRenderSystem();

  ~GLRenderSystem() override;

public: // ISystem

  void OnCreate(registry_t & Registry_) override;

  void OnUpdate(registry_t & Registry_, float Delta_) override;

  void OnDestroy(registry_t & Registry_) override;

protected: // Service

  void UpdateFrustum(const TGlobalTransformComponent & Transform);

  void UpdateUniformBlocks(registry_t & Registry_);

  bool IsNeedUpdateFrustum(registry_t & Registry) const;

  void UpdateMatricesUBO(registry_t & Registry_);

  void UpdateCameraUBO(registry_t & Registry_);

  void UpdateLightUBO(registry_t & Registry_);

  void UpdateShaderUniformBindings(GLuint ShaderID);

  void InitSolidFramebuffer(size_t Width, size_t Height);

  void InitTransparentFramebuffer(size_t Width, size_t Height);

  void InitScreenVAO();

  void InitShadowMap(size_t Width, size_t Height);

  void RenderSolidObjects(registry_t & Registry);

  void RenderTransparentObjects(registry_t & Registry);

  void RenderBackbuffer(registry_t & Registry);

protected: // Members

  CFrustum m_RenderFrustum;

  bool m_IsNeedUpdateFrustum{ true };

  GLuint m_MatricesUBO{ 0 };
  GLuint m_LightUBO{ 0 };
  GLuint m_CameraUBO{ 0 };

  GLuint m_SolidFBO{};
  GLuint m_TransparentFBO{};

  // shadows
  TGLShaderComponent m_DepthShader;
  GLuint m_DepthFBO{};
  GLuint m_DepthTexture{};

  // stuff for order-independent transparency
  GLuint m_SolidTexture{};
  GLuint m_SolidDepthTexture{};
  GLuint m_AccumulatorTexture{};
  GLuint m_RevealTexture{};

  TGLShaderComponent m_CompositeShader;
  TGLShaderComponent m_ScreenShader;

  GLuint m_ScreenQuadVAO{};
};
