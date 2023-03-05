#pragma once
#include <glad/glad.h>
#include "core/entity/System.h"

class GLRenderTransparentPassSystem : public ISystem
{
public: // ISystem

  void OnCreate(registry_t & Registry_) override;

  void OnUpdate(registry_t & Registry_, float Delta_) override;

  void OnDestroy(registry_t & Registry_) override;

protected: // Members

  GLuint m_FBO{};
};
