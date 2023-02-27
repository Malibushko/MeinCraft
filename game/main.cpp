#include "World.h"
#include "components/camera/PerspectiveCameraBundle.h"
#include "components/camera/PerspectiveCameraComponent.h"
#include "components/content/inventory/BlockItemBundle.h"
#include "components/content/inventory/InventoryComponent.h"
#include "components/display/DisplayComponent.h"
#include "components/display/GLFWWindowComponent.h"
#include "components/lightning/DirectedLightBundle.h"
#include "components/render/GLMeshComponent.h"
#include "components/render/GLShaderComponent.h"
#include "components/render/GLTextureComponent.h"
#include "components/render/GLUnbakedMeshComponent.h"
#include "components/terrain/TerrainComponent.h"
#include "components/terrain/VisibleBlockFacesComponent.h"
#include "factory/BlockFactory.h"
#include "resources/ShaderLibrary.h"
#include "resources/TextureLibrary.h"

#include "systems/camera/CameraSystem.h"
#include "systems/content/TargetControllerSystem.h"
#include "systems/content/TargetBlockHighlightSystem.h"
#include "systems/display/GLFWWindowSystem.h"
#include "systems/lightning/DirectedLightMovementSystem.h"
#include "systems/physics/PhysicsSystem.h"
#include "systems/render/GLMeshSystem.h"
#include "systems/render/GLRenderSystem.h"
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
        .Position = { 0.0f, 1.f, 0.0f }
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
        .AddSystem<CPhysicsSystem>();
}

void InitTerrain(World & World_)
{
  World_.Spawn(TTerrainComponent
  {
    .TerrainGenerationStrategy = [](const glm::vec3 & _Position) -> TBlockComponent
    {
      static CFlatTerrainGenerator Generator(time(nullptr));

      return Generator.Generate(_Position);
    }
  });

  World_.AddSystem<CChunkSpawnerSystem>()
        .AddSystem<CChunkDespawnerSystem>()
        .AddSystem<CChunkMeshCullSystem>()
        .AddSystem<CChunkMeshSystem>();
}

void InitLight(World & World_)
{
  World_.SpawnBundle(TDirectedLightBundle
  {
    .Light = TLightComponent
    {
      .Ambient = glm::vec3(0.92f, 0.95f, 0.13f),
    },
    .DirectedLight = TDirectedLightComponent
    {
      .Intensity = 0.3f,
      .Direction = glm::vec3(0.f),
    }
  });

  World_.AddSystem<CDirectedLightMovementSystem>();
}

void InitUI(World & World_)
{
  World_.AddSystem<CNoesisUISystem>()
        .AddSystem<CNoesisUIHUDSystem>();
}

void InitInventory(World & World_)
{
  std::array<entity_t, 3> InitialItems
  {
    World_.Registry().create(),
    World_.Registry().create(),
    World_.Registry().create()
  };

  std::array<EBlockType, 3> BlockTypes =
  {
    EBlockType::Grass,
    EBlockType::CobbleStone,
    EBlockType::Sand
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
  World_.AddSystem<CTargetControllerSystem>();
  World_.AddSystem<CTargetBlockHighlightSystem>();
}
