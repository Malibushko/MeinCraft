#pragma once
#include "core/entity/FrameListenerSystem.h"

struct TGLFWWindowComponent;
struct TDisplayComponent;

class CGLFWWindowSystem : public IFrameListenerSystem
{
public: // Construction/Destruction

  CGLFWWindowSystem();

  ~CGLFWWindowSystem() override;

public: // ISystem

  void OnCreate(registry_t & Registry_) override;

  void OnUpdate(registry_t & Registry_, float Delta_) override;

  void OnDestroy(registry_t & Registry_) override;

  void OnFrameBegin(registry_t& Registry_) override;

  void OnFrameEnd(registry_t& Registry_) override;

protected: // Service

  void InitGLFWWindow(TGLFWWindowComponent & Window, const TDisplayComponent & Display);

  void SendInput(registry_t & Registry_);

protected: // Members

  struct GLFWwindow * m_Window = nullptr;
};
