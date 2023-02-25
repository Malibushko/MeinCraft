#pragma once
#include <string>
#include <NsApp/NotifyPropertyChangedBase.h>
#include <NsCore/BaseComponent.h>
#include <NsCore/String.h>

#include "core/entity/Entity.h"

struct THUDItemsPanelItem : public NoesisApp::NotifyPropertyChangedBase
{
  NS_DECLARE_REFLECTION(THUDItemsPanelItem, NoesisApp::NotifyPropertyChangedBase);

protected: // Members

  Noesis::String     IconPath{};
  entity_t           ItemEntity{};
  int                ItemCount{};
  bool               IsActive{ false };

public: // Interface

  void SetIconPath(const Noesis::String & Path);

  Noesis::String & GetIconPath();

  void SetItemEntity(entity_t Entity);

  entity_t GetItemEntity() const;

  void SetCount(int Count);

  int GetCount() const;

  void SetActive(bool IsActive);

  bool GetActive() const;
};
