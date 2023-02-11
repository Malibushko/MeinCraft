#include "BlockMeshFactory.h"

#include <array>
#include <glad/glad.h>

#include "core/entity/Registry.h"
#include "game/components/render/GLMeshComponent.h"
#include "game/components/render/GLShaderComponent.h"
#include "game/components/render/GLTextureComponent.h"
#include "game/components/terrain/BlockComponent.h"
#include "game/resources/ShaderLibrary.h"
#include "game/resources/TextureLibrary.h"

//
// Service
//

glm::vec2 GetBlockTextureAtlasCoords(EBlockType Type, EBlockFace Face)
{
  switch (Type)
  {
    case EBlockType::GrassDirt:
    {
        switch (Face)
        {
          case EBlockFace::Front:
          case EBlockFace::Back:
          case EBlockFace::Left:
          case EBlockFace::Right:
          {
            return { 16.f, 160.f };
          }
          case EBlockFace::Top:
          {
            return { 224.f, 160.f};
          }
          case EBlockFace::Bottom:
          {
            return { 128.f, 80.f };
          }
        }
    }
  }

  return {};
}

//
// Config
//

static constexpr glm::vec2        TEXTURE_ATLAS_BLOCK_SIZE = { 16.f, 16.f };
static constexpr glm::vec2        TEXTURE_ATLAS_SIZE       = { 512.f, 512.f };
static constexpr std::string_view TEXTURE_ATLAS_PATH       = "res/textures/blocks.png";

static constexpr std::array<glm::vec3, 4> CUBE_FACES[]
{
  {
    // Left
    glm::vec3{ -0.5f, -0.5f, -0.5f },
    glm::vec3{ -0.5f,  0.5f, -0.5f },
    glm::vec3{ -0.5f,  0.5f,  0.5f },
    glm::vec3{ -0.5f, -0.5f,  0.5f },
  },
  {
    // Right
    glm::vec3{  0.5f, -0.5f, -0.5f },
    glm::vec3{  0.5f, -0.5f,  0.5f },
    glm::vec3{  0.5f,  0.5f, 0.5f  },
    glm::vec3{  0.5f,  0.5f, -0.5f },
  },
  {
    // Back
    glm::vec3{ -0.5f, -0.5f, -0.5f },
    glm::vec3{ -0.5f,  0.5f, -0.5f },
    glm::vec3{  0.5f,  0.5f, -0.5f },
    glm::vec3{  0.5f, -0.5f, -0.5f }
  },
  {
     // Front
     glm::vec3{ -0.5f, -0.5f,  0.5f },
     glm::vec3{  0.5f, -0.5f,  0.5f },
     glm::vec3{  0.5f,  0.5f,  0.5f },
     glm::vec3{ -0.5f,  0.5f,  0.5f }
  },
  {
    // Top
    glm::vec3{ -0.5f,  0.5f,  0.5f },
    glm::vec3{  0.5f,  0.5f,  0.5f },
    glm::vec3{  0.5f,  0.5f, -0.5f },
    glm::vec3{ -0.5f,  0.5f, -0.5f },
  },
  {
    // Bottom
    glm::vec3{ -0.5f, -0.5f,  0.5f },
    glm::vec3{ -0.5f, -0.5f, -0.5f },
    glm::vec3{  0.5f, -0.5f, -0.5f },
    glm::vec3{  0.5f, -0.5f,  0.5f }
  }
};

static constexpr std::array<int, 6> CUBE_INDICES[] =
{
  // Front
  {0, 1, 2, 2, 3, 0},
  // Back
  {4, 5, 6, 6, 7, 4},
  // Left
  {8, 9, 10, 10, 11, 8},
  // Right
  {12, 13, 14, 14, 15, 12},
  // Top
  {16, 17, 18, 18, 19, 16},
  // Bottom
  {20, 21, 22, 22, 23, 20}
};

//
// Construction/Destruction
//

CBlockMeshFactory::CBlockMeshFactory() = default;

CBlockMeshFactory::~CBlockMeshFactory() = default;

//
// Service
//

TGLUnbakedMeshComponent CBlockMeshFactory::GetMeshForBlock(const TBlockComponent & Block, EBlockFace Faces) const
{
  TGLUnbakedMeshComponent Mesh;

  int ActiveFaceIndex = 0;

  for (int FaceIndex = 0; FaceIndex < 6; ++FaceIndex)
  {
    if (Faces & (1 << FaceIndex))
    {
      Mesh.Vertices.insert(Mesh.Vertices.end(), CUBE_FACES[FaceIndex].begin(), CUBE_FACES[FaceIndex].end());
      Mesh.Indices.insert(Mesh.Indices.end(), CUBE_INDICES[ActiveFaceIndex].begin(), CUBE_INDICES[ActiveFaceIndex].end());

      ActiveFaceIndex++;

      Mesh.UV = GetUVForBlock(Block, Faces);

      // TODO: Mesh.Normals.push_back(glm::vec3{ 0.0f, 0.0f, 0.0f });
    }
  }

  return Mesh;
}

std::vector<glm::vec2> CBlockMeshFactory::GetUVForBlock(const TBlockComponent & Block, EBlockFace Faces) const
{
  std::vector<glm::vec2> UV;

  for (int FaceIndex = 0; FaceIndex < 6; ++FaceIndex)
  {
    if (Faces & (1 << FaceIndex))
    {
      const EBlockFace Face = static_cast<EBlockFace>(1 << FaceIndex);

      glm::vec2 UVMin = GetBlockTextureAtlasCoords(Block.Type, Face);
      glm::vec2 UVMax = UVMin + TEXTURE_ATLAS_BLOCK_SIZE;

      UVMin /= TEXTURE_ATLAS_SIZE;
      UVMax /= TEXTURE_ATLAS_SIZE;

      switch (Face)
      {
        case EBlockFace::Right:
          UV.insert(UV.end(), { {UVMax.x, UVMax.y}, {UVMin.x, UVMax.y}, {UVMin.x, UVMin.y}, {UVMax.x, UVMin.y}, });
          break;

        case EBlockFace::Left:
          UV.insert(UV.end(), { {UVMax.x, UVMax.y}, {UVMax.x, UVMin.y}, {UVMin.x, UVMin.y}, {UVMin.x, UVMax.y} });
          break;

        case EBlockFace::Top:
        case EBlockFace::Bottom:
          UV.insert(UV.end(), { {UVMax.x, UVMin.y}, {UVMin.x, UVMin.y}, {UVMin.x, UVMax.y}, {UVMax.x, UVMax.y}, });
          break;

        case EBlockFace::Front:
          UV.insert(UV.end(), { {UVMax.x, UVMax.y}, {UVMax.x, UVMin.y}, {UVMin.x, UVMin.y}, {UVMin.x, UVMax.y}, });
          break;

        case EBlockFace::Back:
          UV.insert(UV.end(), { {UVMin.x, UVMax.y}, {UVMax.x, UVMax.y}, {UVMax.x, UVMin.y}, {UVMin.x, UVMin.y} });
          break;
      }
    }
  }

  return UV;
}
