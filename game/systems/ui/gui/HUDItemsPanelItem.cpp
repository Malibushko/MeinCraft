#include "HUDItemsPanelItem.h"

#include <NsCore/ReflectionImplement.h>

NS_IMPLEMENT_REFLECTION(THUDItemsPanelItem)
{
  NsProp("IconPath", &THUDItemsPanelItem::IconPath);
  NsProp("Count",    &THUDItemsPanelItem::ItemCount);
  NsProp("IsActive", &THUDItemsPanelItem::IsActive);
}

//
// Interface
//

void THUDItemsPanelItem::SetIconPath(const Noesis::String & Path)
{
  IconPath = Path;

  OnPropertyChanged("IconPath");
}

Noesis::String & THUDItemsPanelItem::GetIconPath()
{
  return IconPath;
}

void THUDItemsPanelItem::SetItemEntity(entity_t Entity)
{
  ItemEntity = Entity;
}

entity_t THUDItemsPanelItem::GetItemEntity() const
{
  return ItemEntity;
}

void THUDItemsPanelItem::SetCount(int Count)
{
  ItemCount = Count;

  OnPropertyChanged("Count");
}

int THUDItemsPanelItem::GetCount() const
{
  return ItemCount;
}

void THUDItemsPanelItem::SetActive(bool IsActive_)
{
  IsActive = IsActive_;

  OnPropertyChanged("IsActive");
}

bool THUDItemsPanelItem::GetActive() const
{
  return IsActive;
}
