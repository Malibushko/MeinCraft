#include "HUDDataModel.h"

#include <format>
#include <NsCore/ReflectionImplement.h>
#include <fmt/format.h>

#include "game/factory/BlockFactory.h"

NS_IMPLEMENT_REFLECTION(CHUDDataModel)
{
  NsProp("Items",                &CHUDDataModel::m_ItemsPanel);
  NsProp("ActivePanelItem",      &CHUDDataModel::m_ActiveItemsPanelItem);
  NsProp("DebugPosition",        &CHUDDataModel::m_DebugPosition);
  NsProp("DebugTargetPosition",  &CHUDDataModel::m_DebugTargetPosition);
  NsProp("DebugHeight",          &CHUDDataModel::m_DebugHeight);
  NsProp("DebugContinentalness", &CHUDDataModel::m_DebugContinentalness);
  NsProp("DebugErosion",         &CHUDDataModel::m_DebugErosion);
  NsProp("DebugPeaksValleys",    &CHUDDataModel::m_DebugPeaksValleys);
  NsProp("DebugTemperature",     &CHUDDataModel::m_DebugTemperature);
  NsProp("DebugHumidity",        &CHUDDataModel::m_DebugHumidity);
}

//
// Construction/Destruction
//

CHUDDataModel::CHUDDataModel() :
  m_ItemsPanel(*new Noesis::ObservableCollection<THUDItemsPanelItem>)
{
  // Empty
}

CHUDDataModel::~CHUDDataModel()
{
  m_ItemsPanel->Clear();
}

//
// Interface
//

void CHUDDataModel::AddItemsPanelItem(THUDItemsPanelItem * Item)
{
  m_ItemsPanel->Add(Item);

  OnPropertyChanged("Items");
}

THUDItemsPanelItem * CHUDDataModel::GetItemsPanelItem(size_t Index)
{
  return m_ItemsPanel->Get(Index);
}

void CHUDDataModel::SetItemsPanelItem(size_t Index, THUDItemsPanelItem * Item)
{
  m_ItemsPanel->Set(Index, Item);
}

void CHUDDataModel::SetActiveItemsPanelItem(size_t Index)
{
  if (m_ActiveItemsPanelItem)
    m_ActiveItemsPanelItem.GetPtr()->SetActive(false);

  m_ActiveItemsPanelItem.Reset(GetItemsPanelItem(Index));

  if (m_ActiveItemsPanelItem)
    m_ActiveItemsPanelItem.GetPtr()->SetActive(true);
}

size_t CHUDDataModel::GetActiveItemsPanelItemIndex() const
{
  if (!m_ActiveItemsPanelItem)
    return -1;

  return m_ItemsPanel->IndexOf(m_ActiveItemsPanelItem);
}

void CHUDDataModel::SetDebugPosition(glm::vec3 Position)
{
  m_DebugPosition = std::format("Position: ({:.2f}, {:.2f}, {:.2f})", Position.x, Position.y, Position.z).data();

  OnPropertyChanged("DebugPosition");
}

void CHUDDataModel::SetDebugTargetPosition(glm::vec3 TargetPosition)
{
  m_DebugTargetPosition = std::format("Target Position: ({:.2f}, {:.2f}, {:.2f})", TargetPosition.x, TargetPosition.y, TargetPosition.z).data();

  OnPropertyChanged("DebugTargetPosition");
}

void CHUDDataModel::SetDebugBlockInfo(const TTerrainBlockInfo& BlockInfo)
{
  m_DebugHeight          = std::format("Height: {:.2f}", BlockInfo.Height).data();
  m_DebugContinentalness = std::format("Continentalness: {:.2f}", BlockInfo.Continentalness).data();
  m_DebugErosion         = std::format("Erosion: {:.2f}", BlockInfo.Erosion).data();
  m_DebugPeaksValleys    = std::format("PeaksValleys: {:.2f}", BlockInfo.PeaksValleys).data();
  m_DebugTemperature     = std::format("Temperature: {:.2f}", BlockInfo.Temperature).data();
  m_DebugHumidity        = std::format("Humidity: {:.2f}", BlockInfo.Humidity).data();

  OnPropertyChanged("DebugHeight");
  OnPropertyChanged("DebugContinentalness");
  OnPropertyChanged("DebugErosion");
  OnPropertyChanged("DebugPeaksValleys");
  OnPropertyChanged("DebugTemperature");
  OnPropertyChanged("DebugHumidity");
}
