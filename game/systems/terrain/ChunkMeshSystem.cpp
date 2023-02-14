#include "ChunkMeshSystem.h"

#include "core/components/TransformComponent.h"
#include "game/components/physics/BoundingVolume.h"
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
  TGLUnbakedSolidMeshComponent       SolidMeshComponent;
  TGLUnbakedTranslucentMeshComponent TranslucentMeshComponent;

  TTransformComponent ChunkTransform = Registry_.get<TTransformComponent>(ChunkEntity);

  for (int X = 0; X < TChunkComponent::CHUNK_SIZE_X; X++)
  {
    for (int Y = 0; Y < TChunkComponent::CHUNK_SIZE_Y; Y++)
    {
      for (int Z = 0; Z < TChunkComponent::CHUNK_SIZE_Z; Z++)
      {
        const int Index = X + TChunkComponent::CHUNK_SIZE_X * (Y + TChunkComponent::CHUNK_SIZE_Y * Z);

        if (Chunk.Blocks[Index] == entt::null)
          continue;

        auto && [Block, Faces] = Registry_.get<TBlockComponent, TVisibleBlockFacesComponent>(Chunk.Blocks[Index]);

        if (Faces.Faces == EBlockFace::None)
          continue;

        TGLUnbakedSolidMeshComponent BlockMesh     = CBlockFactory::GetMeshForBlock(Block, Faces.Faces);
        std::vector<glm::vec2>       BlockUV       = CBlockFactory::GetUVForBlock(Block, Faces.Faces);
        EMeshType                    BlockMeshType = CBlockFactory::GetMeshTypeForBlock(Block);

        const auto AppendToMesh = [&]<EMeshType T>(TGLUnbakedMeshComponent<T> & ChunkMesh)
        {
          for (auto & Vertex : BlockMesh.Vertices)
            Vertex += glm::vec3(X, Y, Z);

          for (auto & Index : BlockMesh.Indices)
            Index += ChunkMesh.Vertices.size();

          ChunkMesh.Vertices.insert(ChunkMesh.Vertices.end(), BlockMesh.Vertices.begin(), BlockMesh.Vertices.end());
          ChunkMesh.Indices.insert(ChunkMesh.Indices.end(), BlockMesh.Indices.begin(), BlockMesh.Indices.end());
          ChunkMesh.UV.insert(ChunkMesh.UV.end(), BlockUV.begin(), BlockUV.end());
          ChunkMesh.Normals.insert(ChunkMesh.Normals.end(), BlockMesh.Normals.begin(), BlockMesh.Normals.end());
        };

        switch (BlockMeshType)
        {
          case EMeshType::Translucent:
            AppendToMesh(TranslucentMeshComponent);
            break;

          default:
            AppendToMesh(SolidMeshComponent);
            break;
        }
      }
    }
  }

  TGLShaderComponent Shader
  {
    .ShaderID = CShaderLibrary::Load("res/shaders/blocks_shader").ShaderID
  };

  TGLTextureComponent Texture
  {
    .TextureID = CTextureLibrary::Load("res/textures/blocks_atlas.png").TextureID
  };

  // TODO: This is a hack, we should be able to get the AABB from the mesh
  Registry_.emplace_or_replace<TBoundingVolumeComponent>(ChunkEntity, TBoundingVolumeComponent
  {
    .Volume = TAABBVolumeComponent
    {
      .Min = ChunkTransform.Transform[3],
      .Max = glm::vec3(ChunkTransform.Transform[3]) + glm::vec3(TChunkComponent::CHUNK_SIZE_X, TChunkComponent::CHUNK_SIZE_Y, TChunkComponent::CHUNK_SIZE_Z)
    }
  });

  if (!SolidMeshComponent.Vertices.empty())
    Registry_.emplace_or_replace<TGLUnbakedSolidMeshComponent>(ChunkEntity, std::move(SolidMeshComponent));

  if (!TranslucentMeshComponent.Vertices.empty())
    Registry_.emplace_or_replace<TGLUnbakedTranslucentMeshComponent>(ChunkEntity, std::move(TranslucentMeshComponent));

  Registry_.emplace_or_replace<TGLShaderComponent>(ChunkEntity, Shader);
  Registry_.emplace_or_replace<TGLTextureComponent>(ChunkEntity, Texture);
}

