#include <iostream>
#include <core/entity/entity.h>
#include <glm/trigonometric.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "core/components/PositionComponent.h"

#include "World.h"
#include "components/camera/PerspectiveCameraBundle.h"
#include "components/camera/PerspectiveCameraComponent.h"
#include "components/display/DisplayComponent.h"
#include "components/display/GLFWWindowComponent.h"
#include "components/render/GLUnbakedMeshComponent.h"
#include "components/render/GLShaderComponent.h"
#include "components/render/GLTextureComponent.h"
#include "resources/ShaderLibrary.h"
#include "resources/TextureLibrary.h"

#include "systems/camera/CameraSystem.h"
#include "systems/display/GLFWWindowSystem.h"
#include "systems/render/GLMeshSystem.h"
#include "systems/render/GLRenderSystem.h"

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
    .Camera = TCameraBundle
    {
      .Position = TPositionComponent
      {
        .Position = { 0.0f, 0.0f, -3.0f }
      },

      .Basis = TCameraBasisComponent
      {
        .Up    = { 0.0f, 1.0f, 0.0f  },
        .Front = { 0.0f, 0.0f, -1.0f },
        .Right = { 1.0f, 0.0f, 0.0f  }
      }
    },
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
        .AddSystem<GLRenderSystem>()
        .AddSystem<CCameraSystem>()
        .AddSystem<GLMeshSystem>();
}

void InitTerrain(World & World_)
{
  struct TriangleSpawner : ISystem
  {
    struct Triangle
    {
      TGLUnbakedMeshComponent Mesh;
      TGLShaderComponent      Shader;
      TGLTextureComponent     Texture;
      TPositionComponent      Position;
    };

    void OnCreate(registry_t & Registry_) override
    {
      Triangle Triangle;

      Triangle.Shader.ShaderID   = CShaderLibrary::Load("res/shaders/basic").ShaderID;
      Triangle.Texture.TextureID = CTextureLibrary::Load("res/textures/mosaic.png").TextureID;
      Triangle.Mesh.Vertices     =
      {
        { -0.5f, -0.5f, 0.0f },
        {  0.5f, -0.5f, 0.0f },
        {  0.0f,  0.5f, 0.0f }
      };

      Triangle.Mesh.Indices =
      {
        0, 1, 2
      };

      Triangle.Mesh.UV =
      {
        { 0.0f, 0.0f },
        { 1.0f, 0.0f },
        { 0.5f, 1.0f }
      };

      Triangle.Position.Position = { 0.0f, 0.0f, -1.0f };

      SpawnBundle(Registry_, std::move(Triangle));
    }

    void OnUpdate(registry_t& Registry_, float Delta_) override
    {
      // Empty
    }

    void OnDestroy(registry_t& Registry_) override
    {
      // Empty
    }
  };

  World_.AddSystem<TriangleSpawner>();
}
