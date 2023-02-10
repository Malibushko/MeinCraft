#include "World.h"
#include "components/camera/PerspectiveCameraBundle.h"
#include "components/camera/PerspectiveCameraComponent.h"
#include "components/display/DisplayComponent.h"
#include "components/display/GLFWWindowComponent.h"
#include "components/render/GLUnbakedMeshComponent.h"
#include "components/terrain/TerrainComponent.h"

#include "systems/camera/CameraSystem.h"
#include "systems/display/GLFWWindowSystem.h"
#include "systems/render/GLMeshSystem.h"
#include "systems/render/GLRenderSystem.h"
#include "systems/terrain/ChunkDespawnerSystem.h"
#include "systems/terrain/ChunkMeshSystem.h"
#include "systems/terrain/ChunkSpawnerSystem.h"

#include "utils/NumericUtils.h"

void InitCamera (World & World_);
void InitDisplay(World & World_);
void InitSystems(World & World_);
void InitTerrain(World & World_);

int main()
{
  World World;

  World.AddStartupFunction(InitDisplay)
       .AddStartupFunction(InitCamera)
       .AddStartupFunction(InitSystems)
       .AddStartupFunction(InitTerrain);

  World.Run();
}

void InitCamera(World & World_)
{
  TPerspectiveCameraBundle Bundle;

  World_.SpawnBundle(TPerspectiveCameraBundle
  {
    .Perspective =
    {
      .FOV         = 45.0f,
      .AspectRatio = QueryFirst<TDisplayComponent>(World_.Registry()).GetAspectRatio(),
      .Near        = 0.1f,
      .Far         = 100.f,
    }
  });
}

void InitDisplay(World & World_)
{
  World_.Spawn(TDisplayComponent
  {
    .Width  = 800,
    .Height = 600
  })
  .Spawn(TGLFWWindowComponent
  {
    .Title  = "MeinCraft",
    .Window = nullptr
  });
}

void InitSystems(World & World_)
{
  World_.AddSystem<CGLFWWindowSystem>()
        .AddSystem<GLMeshSystem>()
        .AddSystem<GLRenderSystem>()
        .AddSystem<CCameraSystem>();
}

void InitTerrain(World & World_)
{
  World_.Spawn(TTerrainComponent
  {
    .TerrainGenerationStrategy = [](const glm::vec3 & _Position) -> TBlockComponent
    {
      if (Utils::AlmostEqual(_Position.y, 0.f))
        return TBlockComponent{ .Type = EBlockType::GrassDirt };

      return TBlockComponent{ .Type = EBlockType::Air };
    }
  });

  World_.AddSystem<CChunkSpawnerSystem>()
        .AddSystem<CChunkDespawnerSystem>()
        .AddSystem<CChunkMeshSystem>();
}
