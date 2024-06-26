#include "World.h"
#include "components/camera/PerspectiveCameraBundle.h"
#include "components/camera/PerspectiveCameraComponent.h"
#include "components/content/inventory/BlockItemBundle.h"
#include "components/content/inventory/InventoryComponent.h"
#include "components/display/DisplayComponent.h"
#include "components/display/GLFWWindowComponent.h"
#include "components/lightning/DirectedLightBundle.h"
#include "components/render/GLTextureComponent.h"
#include "components/terrain/TerrainComponent.h"
#include "factory/BlockFactory.h"

#include "systems/camera/CameraSystem.h"

#include "systems/content/BlockItemUseSystem.h"
#include "systems/content/InventorySystem.h"
#include "systems/content/PlayerTargetActionSystem.h"
#include "systems/content/TargetControllerSystem.h"
#include "systems/content/TargetBlockHighlightSystem.h"

#include "systems/display/GLFWWindowSystem.h"

#include "systems/lightning/DirectedLightMeshSystem.h"
#include "systems/lightning/DirectedLightMovementSystem.h"

#include "systems/physics/PhysicsSystem.h"

#include "systems/render/GLMeshSystem.h"
#include "systems/render/GLRenderBloomPassSystem.h"
#include "systems/render/GLRenderDirectedLightDepthPassSystem.h"
#include "systems/render/GLRenderLightAccumulationPassSystem.h"
#include "systems/render/GLRenderScreenPassSystem.h"
#include "systems/render/GLRenderSolidPassSystem.h"
#include "systems/render/GLRenderSystem.h"
#include "systems/render/GLRenderTransparentPassSystem.h"
#include "systems/render/GLRenderBufferObjectsSystem.h"
#include "systems/render/GLRenderCompositePassSystem.h"
#include "systems/render/GLRenderPostEffectsPassSystem.h"
#include "systems/render/GLRenderSkyboxSystem.h"
#include "systems/render/GLRenderSSAOPassSystem.h"

#include "systems/requests/RequestsSystem.h"

#include "systems/terrain/ChunkBlocksControllerSystem.h"
#include "systems/terrain/ChunkDespawnerSystem.h"
#include "systems/terrain/ChunkMeshCullSystem.h"
#include "systems/terrain/ChunkMeshSystem.h"
#include "systems/terrain/ChunkSpawnerSystem.h"

#include "systems/terrain/NoiseTerrainGenerator.h"
#include "systems/terrain/FlatTerrainGenerator.h"

#include "systems/ui/NoesisUISystem.h"
#include "systems/ui/gui/NoesisUIHUDSystem.h"

#include "systems/utils/FPSCounterSystem.h"

#include "utils/NumericUtils.h"

void InitCamera (World & World_);
void InitDisplay(World & World_);
void InitCoreSystems(World & World_);
void InitTerrain(World & World_);
void InitLight(World & World_);
void InitUI(World & World_);
void InitInventory(World & World_);
void InitMetrics(World & World_);
void InitGameplayFeatures(World & World_);

int main()
{
  World World;

  World.AddStartupFunction(InitDisplay)
       .AddStartupFunction(InitCamera)
       .AddStartupFunction(InitCoreSystems)
       .AddStartupFunction(InitTerrain)
       .AddStartupFunction(InitLight)
       .AddStartupFunction(InitInventory)
       .AddStartupFunction(InitUI)
       .AddStartupFunction(InitGameplayFeatures)
       .AddStartupFunction(InitMetrics);
  World.Run();
}

void InitCamera(World & World_)
{
  TPerspectiveCameraBundle Bundle;

  World_.SpawnBundle(TPerspectiveCameraBundle
  {
    .Camera = TCameraBundle
    {
      .Position = TPositionComponent{
        .Position = { 0.0f, 2.f, 0.0f }
      }
    },
    .Perspective =
    {
      .FOV         = 45.0f,
      .AspectRatio = QuerySingle<TDisplayComponent>(World_.Registry()).GetAspectRatio(),
      .Near        = 0.1f,
      .Far         = 1000.f,
    }
  });
}

void InitDisplay(World & World_)
{
  World_.Spawn(TDisplayComponent
  {
    .Width  = 1280,
    .Height = 720
  })
  .Spawn(TGLFWWindowComponent
  {
    .Title  = "MeinCraft",
    .Icon   = "res/icons/logo.png",
    .Window = nullptr,
  });
}

void InitCoreSystems(World & World_)
{
  World_.AddSystem<CGLFWWindowSystem>()
        .AddSystem<GLMeshSystem>()
        .AddSystem<CCameraSystem>()

        .AddSystem<GLRenderSystem>()
        .AddSystem<GLRenderBufferObjectsSystem>()
        .AddSystem<GLRenderDirectedLightDepthPassSystem>()
        .AddSystem<GLRenderLightAccumulationPassSystem>()
        .AddSystem<GLRenderSolidPassSystem>()
        .AddSystem<GLRenderTransparentPassSystem>()
        .AddSystem<GLRenderSkyboxSystem>()
        .AddSystem<GLRenderSSAOPassSystem>()
        .AddSystem<GLRenderCompositePassSystem>()
        .AddSystem<GLRenderBloomPassSystem>()
        .AddSystem<GLRenderPostEffectsPassSystem>()
        .AddSystem<GLRenderScreenPassSystem>()

        .AddSystem<CPhysicsSystem>()
        .AddSystem<CRequestsSystem>();
}

void InitTerrain(World & World_)
{
  World_.Spawn(TTerrainComponent
  {
    .TerrainGenerationStrategy = [&](const glm::vec3 & _Position) -> TBlockComponent
    {
      static CNoiseTerrainGenerator Generator(time(nullptr));

      return Generator.Generate(_Position);
    }
  });

  World_.AddSystem<CChunkSpawnerSystem>()
        .AddSystem<CChunkDespawnerSystem>()
        .AddSystem<CChunkBlocksControllerSystem>()
        .AddSystem<CChunkMeshCullSystem>()
        .AddSystem<CChunkMeshSystem>();
}

void InitLight(World & World_)
{
  World_.SpawnBundle(TDirectedLightBundle
  {
    .Light = TLightComponent
    {
      .Color = glm::vec4(0.02f, 0.25f, 0.47f, 1.f),
    },
    .DirectedLight = TDirectedLightComponent
    {
      .Intensity = 0.3f,
      .Direction = glm::vec3(0.f),
    }
  });

  World_.AddSystem<CDirectedLightMovementSystem>()
        .AddSystem<CDirectedLightMeshSystem>();
}

void InitUI(World & World_)
{
  World_.AddSystem<CNoesisUISystem>()
        .AddSystem<CNoesisUIHUDSystem>();
}

void InitInventory(World & World_)
{
  std::array InitialItems
  {
    World_.Registry().create(),
    World_.Registry().create(),
    World_.Registry().create(),
    World_.Registry().create(),
  };

  std::array BlockTypes =
  {
    EBlockType::GrassBlock,
    EBlockType::CobbleStone,
    EBlockType::Sand,
    EBlockType::GlowStone
  };

  for (int i = 0; i < InitialItems.size(); i++)
  {
    AddBundle(World_.Registry(), InitialItems[i], TBlockItemBundle
    {
      .ItemData = TItemComponent{.Count = 10, .StackSize = 64},
      .Block    = TBlockComponent{.Type = BlockTypes[i] }
    });
  }

  TInventoryComponent Inventory;

  Inventory.Inventory .fill(entt::null);
  Inventory.ItemsPanel.fill(entt::null);

  std::ranges::copy(std::as_const(InitialItems), Inventory.ItemsPanel.begin());

  World_.Spawn<TInventoryComponent>(std::move(Inventory));
}

void InitMetrics(World & World_)
{
  World_.AddSystem<CFPSCounterSystem>();
}

void InitGameplayFeatures(World& World_)
{
  World_.AddSystem<CTargetControllerSystem>()
        .AddSystem<CPlayerTargetActionSystem>()
        .AddSystem<CTargetBlockHighlightSystem>()
        .AddSystem<CInventorySystem>()
        .AddSystem<CBlockItemUseSystem>();
}
