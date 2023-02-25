#include "HUDDataModel.h"

#include <NsCore/ReflectionImplement.h>

#include "game/factory/BlockFactory.h"

NS_IMPLEMENT_REFLECTION(CHUDDataModel)
{
  NsProp("Items",           &CHUDDataModel::m_ItemsPanel);
  NsProp("ActivePanelItem", &CHUDDataModel::m_ActiveItemsPanelItem);
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
