#pragma once
#include "core/entity/System.h"
#include <NsRender/GLFactory.h>
#include <NsGui/FontProperties.h>
#include <NsGui/IntegrationAPI.h>
#include <NsGui/IRenderer.h>
#include <NsGui/IView.h>
#include <NsGui/Grid.h>
#include <NsApp/NotifyPropertyChangedBase.h>

#include "HUDDataModel.h"

class CNoesisUIHUDSystem : public ISystem
{
public: // Construction/Desturction

  CNoesisUIHUDSystem();

  ~CNoesisUIHUDSystem();

public: // Interface

  void OnCreate(registry_t & Registry_) override;

  void OnUpdate(registry_t & Registry_, float Delta_) override;

  void OnDestroy(registry_t & Registry_) override;

protected: // Service

  void InitItemsPanel(registry_t & Registry);

  void ProcessInput(registry_t & Registry);

  void UpdateDebugPanel(registry_t & Registry);

protected: // Members

  Noesis::Ptr<Noesis::IView> m_View;
  Noesis::Ptr<CHUDDataModel> m_DataModel;
};
