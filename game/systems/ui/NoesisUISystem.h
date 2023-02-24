#pragma once
#include "core/entity/System.h"
#include <NsRender/GLFactory.h>
#include <NsGui/FontProperties.h>
#include <NsGui/IntegrationAPI.h>
#include <NsGui/IRenderer.h>
#include <NsGui/IView.h>
#include <NsGui/Grid.h>

class CNoesisUISystem : public ISystem
{
public: // Construction/Desturction

  CNoesisUISystem();

  ~CNoesisUISystem();

public: // Interface

  void OnCreate(registry_t & Registry_) override;

  void OnUpdate(registry_t & Registry_, float Delta_) override;

  void OnDestroy(registry_t & Registry_) override;

protected: // Members

  Noesis::Ptr<Noesis::IView> m_View;
};
