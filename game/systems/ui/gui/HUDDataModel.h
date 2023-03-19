#pragma once
#include <string>
#include <glm/vec3.hpp>
#include <NsApp/NotifyPropertyChangedBase.h>
#include <NsCore/BaseComponent.h>
#include <NsGui/ObservableCollection.h>
#include "HUDItemsPanelItem.h"
#include "game/components/terrain/TerrainMap.h"

class CHUDDataModel final : public NoesisApp::NotifyPropertyChangedBase
{
public: // Construction/Destruction

  CHUDDataModel();

  ~CHUDDataModel();

public: // Interface

  void AddItemsPanelItem(THUDItemsPanelItem* Item);

  THUDItemsPanelItem * GetItemsPanelItem(size_t Index);

  void SetItemsPanelItem(size_t Index, THUDItemsPanelItem * Item);

  void SetActiveItemsPanelItem(size_t Index);

  size_t GetActiveItemsPanelItemIndex() const;

public: // Debug interface

  void SetDebugPosition(glm::vec3 Position);

  void SetDebugTargetPosition(glm::vec3 TargetPosition);

  void SetDebugBlockInfo(const TTerrainBlockInfo & BlockInfo);

private: // Members

  Noesis::Ptr<Noesis::ObservableCollection<THUDItemsPanelItem>> m_ItemsPanel;
  Noesis::Ptr<THUDItemsPanelItem>                               m_ActiveItemsPanelItem;

  Noesis::String m_DebugPosition;
  Noesis::String m_DebugTargetPosition;
  Noesis::String m_DebugHeight;
  Noesis::String m_DebugContinentalness;
  Noesis::String m_DebugErosion;
  Noesis::String m_DebugPeaksValleys;
  Noesis::String m_DebugTemperature;
  Noesis::String m_DebugHumidity;

  NS_DECLARE_REFLECTION(CHUDDataModel, NotifyPropertyChangedBase)
};
