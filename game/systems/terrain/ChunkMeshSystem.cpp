#include "ChunkMeshSystem.h"

#include "core/components/PositionComponent.h"
#include "core/components/TransformComponent.h"
#include "game/components/lightning/LightComponent.h"
#include "game/components/lightning/PointLightComponent.h"
#include "game/components/physics/BoundingVolume.h"
#include "game/components/tags/NoShadowCastTag.h"
#include "game/components/terrain/BlockComponent.h"
#include "game/components/terrain/ChunkComponent.h"
#include "game/factory/BlockFactory.h"
#include "game/resources/ShaderLibrary.h"
#include "game/resources/TextureLibrary.h"

//
// Construction/Destruction
//

CChunkMeshSystem::CChunkMeshSystem() = default;

CChunkMeshSystem::~CChunkMeshSystem() = default;

//
// ISystem
//

void CChunkMeshSystem::OnCreate(registry_t & Registry_)
{
  // Empty
}

void CChunkMeshSystem::OnUpdate(registry_t & Registry_, float Delta_)
{
  for (auto && [Entity, Chunk] : Registry_.view<TChunkComponent>().each())
  {
    if (Chunk.State == EChunkState::Clear)
      continue;

    RecreateChunkMesh(Registry_, Entity, Chunk);

    Chunk.State = EChunkState::Clear;
  }
}

void CChunkMeshSystem::OnDestroy(registry_t & Registry_)
{
  // Empty
}

void CChunkMeshSystem::RecreateChunkMesh(registry_t & Registry_, entity_t ChunkEntity, TChunkComponent & Chunk) const
{
  DestroyChunkMesh(Registry_, Chunk);

  std::unordered_map<int, TGLUnbakedSolidMeshComponent      > SolidMeshComponents;
  std::unordered_map<int, TGLUnbakedTranslucentMeshComponent> TranslucentMeshComponents;

  TGLShaderComponent SolidMeshShader, TransclucentMeshShader;

  TTransformComponent ChunkTransform = Registry_.get<TTransformComponent>(ChunkEntity);

  const glm::vec3 ChunkPosition = ChunkTransform.Transform[3];

  for (size_t Index = 0; Index < TChunkComponent::BLOCKS_COUNT; Index++)
  {
    if (Chunk.Blocks[Index] == entt::null)
      continue;

    auto && [Block, Faces] = Registry_.get<TBlockComponent, TVisibleBlockFacesComponent>(Chunk.Blocks[Index]);

    if (Faces.Faces == EBlockFace::None)
      continue;

    TGLUnbakedSolidMeshComponent BlockMesh     = CBlockFactory::GetMeshForBlock(Block, Faces.Faces);
    EMeshType                    BlockMeshType = CBlockFactory::GetMeshTypeForBlock(Block);
    const glm::ivec3             BlockPosition = BlockIndexToPosition(Index);

    if (const int Factor = CBlockFactory::GetBlockEmitLightFactor(Block); Factor > 0)
    {
      if (Registry_.try_get<TLightComponent>(Chunk.Blocks[Index]) == nullptr)
      {
        // Offset by 0.5 on all axes to center the light in the block
        const glm::vec3 LightPosition = glm::vec3(ChunkPosition) + glm::vec3(BlockPosition) + glm::vec3(0.5);

        AddComponent<TPositionComponent>(Registry_, Chunk.Blocks[Index], TPositionComponent
        {
          .Position = LightPosition
        });

        AddComponent<TLightComponent>(Registry_, Chunk.Blocks[Index], CBlockFactory::GetBlockLightComponent(Block));
        AddComponent<TPointLightComponent>(Registry_, Chunk.Blocks[Index], CBlockFactory::GetBlockPointLightComponent(Block));
      }
    }

    const auto AppendToMesh = [&]<EMeshType T>(TGLUnbakedMeshComponent<T> & ChunkMesh)
    {
      for (auto & Vertex : BlockMesh.Vertices)
        Vertex += BlockPosition;

      for (auto & Index : BlockMesh.Indices)
        Index += static_cast<unsigned short>(ChunkMesh.Vertices.size());

      ChunkMesh.Vertices.insert(ChunkMesh.Vertices.end(), BlockMesh.Vertices.begin(), BlockMesh.Vertices.end());
      ChunkMesh.Indices.insert(ChunkMesh.Indices.end(), BlockMesh.Indices.begin(), BlockMesh.Indices.end());
      ChunkMesh.UV.insert(ChunkMesh.UV.end(), BlockMesh.UV.begin(), BlockMesh.UV.end());
      ChunkMesh.Normals.insert(ChunkMesh.Normals.end(), BlockMesh.Normals.begin(), BlockMesh.Normals.end());
    };


    switch (BlockMeshType)
    {
      case EMeshType::Translucent:
        AppendToMesh(TranslucentMeshComponents[BlockMesh.MaterialID]);

        TransclucentMeshShader = CBlockFactory::GetShaderForBlock(Block);
        break;

      default:
        AppendToMesh(SolidMeshComponents[BlockMesh.MaterialID]);

        SolidMeshShader = CBlockFactory::GetShaderForBlock(Block);
        break;
    }
  }

  TGLTextureComponent Texture
  {
    .TextureID = CTextureLibrary::Load("res/textures/blocks_atlas.png").TextureID
  };

  for (auto & [MaterialID, Mesh] : SolidMeshComponents)
  {
    entity_t SolidMeshEntity = Registry_.create();

    Mesh.MaterialID = MaterialID;

    auto [Min, Max] = Mesh.GetBounds();

    Registry_.emplace<TGLUnbakedSolidMeshComponent>(SolidMeshEntity, std::move(Mesh));
    Registry_.emplace<TGLShaderComponent>(SolidMeshEntity, SolidMeshShader);
    Registry_.emplace<TGLTextureComponent>(SolidMeshEntity, Texture);
    Registry_.emplace<TBoundingVolumeComponent>(SolidMeshEntity, TBoundingVolumeComponent
      {
        .Volume = TAABBVolumeComponent{.Min = Min + glm::vec3(ChunkTransform.Transform[3]), .Max = Max + glm::vec3(ChunkTransform.Transform[3])}
      });

    Registry_.emplace<TTransformComponent>(SolidMeshEntity, ChunkTransform);

    Chunk.Meshes.emplace_back(SolidMeshEntity);
  }

  for (auto & [MaterialID, Mesh] : TranslucentMeshComponents)
  {
    entity_t TranslucentMeshEntity = Registry_.create();

    Mesh.MaterialID = MaterialID;

    auto [Min, Max] = Mesh.GetBounds();

    Registry_.emplace<TGLUnbakedTranslucentMeshComponent>(TranslucentMeshEntity, std::move(Mesh));
    Registry_.emplace<TGLShaderComponent>(TranslucentMeshEntity, TransclucentMeshShader);
    Registry_.emplace<TGLTextureComponent>(TranslucentMeshEntity, Texture);
    Registry_.emplace<TBoundingVolumeComponent>(TranslucentMeshEntity, TBoundingVolumeComponent
    {
      .Volume = TAABBVolumeComponent{.Min = Min + glm::vec3(ChunkTransform.Transform[3]), .Max = Max + glm::vec3(ChunkTransform.Transform[3])}
    });

    Registry_.emplace<TTransformComponent>(TranslucentMeshEntity, ChunkTransform);

    Chunk.Meshes.emplace_back(TranslucentMeshEntity);
  }
}

void CChunkMeshSystem::DestroyChunkMesh(registry_t & Registry, TChunkComponent & Chunk) const
{
  for (const auto & Mesh : Chunk.Meshes)
    Registry.destroy(Mesh);

  Chunk.Meshes.clear();
}

