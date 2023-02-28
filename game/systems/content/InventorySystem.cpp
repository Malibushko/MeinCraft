#include "InventorySystem.h"

#include "game/factory/BlockFactory.h"

//
// Construction/Destruction
//

CInventorySystem::CInventorySystem() = default;

CInventorySystem::~CInventorySystem() = default;

//
// ISystem
//

void CInventorySystem::OnCreate(registry_t & Registry_)
{
  // Empty
}

void CInventorySystem::OnUpdate(registry_t & Registry_, float Delta_)
{

}


void CInventorySystem::OnDestroy(registry_t & Registry_)
{
  // Empty
}
