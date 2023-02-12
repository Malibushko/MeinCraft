#include "WorldLightMeshSystem.h"

#include "core/components/TransformComponent.h"
#include "game/components/lightning/DirectedLightComponent.h"
#include "game/components/render/GLMeshComponent.h"
#include "game/components/terrain/BlockComponent.h"
#include "game/factory/BlockMeshFactory.h"
#include "game/resources/ShaderLibrary.h"
#include "game/resources/TextureLibrary.h"

//
// Construction/Destruction
//

CWorldLightMeshSystem::CWorldLightMeshSystem() = default;

CWorldLightMeshSystem::~CWorldLightMeshSystem() = default;

//
// ISystem
//

void CWorldLightMeshSystem::OnCreate(registry_t& Registry_)
{
  // Empty
}

void CWorldLightMeshSystem::OnUpdate(registry_t & Registry_, float Delta_)
{
  auto View = Registry_.view<TTransformComponent, TDirectedLightComponent>(entt::exclude_t<TGLMeshComponent>{});

  for (auto && [Entity, Transform, Light] : View.each())
  {
    InitWorldLightMesh(Registry_, Entity);
  }
}

void CWorldLightMeshSystem::OnDestroy(registry_t& Registry_)
{
  // Empty
}

void CWorldLightMeshSystem::InitWorldLightMesh(registry_t & Registry, entity_t LightEntity)
{
  TBlockComponent Block
  {
    .Type = EBlockType::Sun
  };

  Registry.emplace<TBlockComponent>(LightEntity, Block);

  Registry.emplace<TGLMeshComponent>(LightEntity, CBlockMeshFactory::GetMeshForBlock(Block, EBlockFace::All));

  Registry.emplace<TGLShaderComponent>(LightEntity, TGLShaderComponent
  {
    .ShaderID = CShaderLibrary::Load("res/shaders/blocks_shader").ShaderID
  });

  Registry.emplace<TGLTextureComponent>(LightEntity, TGLTextureComponent
  {
    .TextureID = CTextureLibrary::Load("res/textures/blocks_atlas.png").TextureID
  });
}
