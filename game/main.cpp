#include "World.h"
#include "components/camera/PerspectiveCameraBundle.h"
#include "components/camera/PerspectiveCameraComponent.h"
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
#include "systems/display/GLFWWindowSystem.h"
#include "systems/lightning/DirectedLightMovementSystem.h"
#include "systems/render/GLMeshSystem.h"
#include "systems/render/GLRenderSystem.h"
#include "systems/terrain/ChunkDespawnerSystem.h"
#include "systems/terrain/ChunkMeshCullSystem.h"
#include "systems/terrain/ChunkMeshSystem.h"
#include "systems/terrain/ChunkSpawnerSystem.h"
#include "systems/terrain/NoiseTerrainGenerator.h"
#include "systems/ui/ultralight/UltraLightLoggerSystem.h"
#include "systems/ui/ultralight/UltraLightUIRenderSystem.h"
#include "systems/ui/ultralight/UltraLightUISystem.h"
#include "systems/utils/FPSCounterSystem.h"

#include "utils/NumericUtils.h"

void InitCamera (World & World_);
void InitDisplay(World & World_);
void InitCoreSystems(World & World_);
void InitTerrain(World & World_);
void InitLight(World & World_);
void InitUI(World & World_);
void InitMetrics(World & World_);

int main()
{
  World World;

  World.AddStartupFunction(InitDisplay)
       .AddStartupFunction(InitCamera)
       .AddStartupFunction(InitCoreSystems)
       .AddStartupFunction(InitTerrain)
       .AddStartupFunction(InitLight)
       .AddStartupFunction(InitUI)
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
        .Position = { 0.0f, 65.f, 0.0f }
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
        .AddSystem<GLRenderSystem>();
}

void InitTerrain(World & World_)
{
  World_.Spawn(TTerrainComponent
  {
    .TerrainGenerationStrategy = [](const glm::vec3 & _Position) -> TBlockComponent
    {
      static CNoiseTerrainGenerator Generator(time(nullptr));

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
  World_.AddSystem<CUltraLightUIRenderSystem>();
  World_.AddSystem<CUltraLightUISystem>();
  World_.AddSystem<CUltraLightUILoggerSystem>();
}

void InitMetrics(World & World_)
{
  World_.AddSystem<CFPSCounterSystem>();
}
