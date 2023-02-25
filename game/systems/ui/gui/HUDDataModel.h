#pragma once
#include <string>
#include <NsApp/NotifyPropertyChangedBase.h>
#include <NsCore/BaseComponent.h>
#include <NsGui/ObservableCollection.h>
#include "HUDItemsPanelItem.h"

class CHUDDataModel final : public NoesisApp::NotifyPropertyChangedBase
{
public: // Construction/Destruction

  CHUDDataModel();

  ~CHUDDataModel();

public: // Interface

  void AddItemsPanelItem(THUDItemsPanelItem* Item);

  THUDItemsPanelItem * GetItemsPanelItem(size_t Index);

  void SetActiveItemsPanelItem(size_t Index);

  size_t GetActiveItemsPanelItemIndex() const;

private: // Members

  Noesis::Ptr<Noesis::ObservableCollection<THUDItemsPanelItem>> m_ItemsPanel;
  Noesis::Ptr<THUDItemsPanelItem>                               m_ActiveItemsPanelItem;

  NS_DECLARE_REFLECTION(CHUDDataModel, NotifyPropertyChangedBase)
};