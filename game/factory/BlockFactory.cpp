#include "BlockFactory.h"

#include <array>
#include <fstream>
#include <glad/glad.h>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include "core/entity/Registry.h"
#include "game/components/render/GLMeshComponent.h"
#include "game/components/render/GLShaderComponent.h"
#include "game/components/render/GLTextureComponent.h"
#include "game/components/terrain/BlockComponent.h"
#include "game/resources/ShaderLibrary.h"
#include "game/resources/TextureLibrary.h"


//
// Config
//

static constexpr std::string_view BLOCK_UV_CONFIG_PATH   = "res/configs/blocks_uv.json";
static constexpr std::string_view BLOCK_INFO_CONFIG_PATH = "res/configs/blocks.json";

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
    // Front
    glm::vec3{ -0.5f, -0.5f, -0.5f },
    glm::vec3{ -0.5f,  0.5f, -0.5f },
    glm::vec3{  0.5f,  0.5f, -0.5f },
    glm::vec3{  0.5f, -0.5f, -0.5f }
  },
  {
     // Back
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

static constexpr std::array<glm::vec3, 4> CUBE_NORMALS[]  =
{
  // Left
  {
    glm::vec3{ -1.f,  0.f,  0.f },
    glm::vec3{ -1.f,  0.f,  0.f },
    glm::vec3{ -1.f,  0.f,  0.f },
    glm::vec3{ -1.f,  0.f,  0.f },
  },
  // Right
  {
    glm::vec3{  1.f,  0.f,  0.f },
    glm::vec3{  1.f,  0.f,  0.f },
    glm::vec3{  1.f,  0.f,  0.f },
    glm::vec3{  1.f,  0.f,  0.f },
  },
  // Back
  {
    glm::vec3{  0.f,  0.f, -1.f },
    glm::vec3{  0.f,  0.f, -1.f },
    glm::vec3{  0.f,  0.f, -1.f },
    glm::vec3{  0.f,  0.f, -1.f },
  },
  // Front
  {
    glm::vec3{  0.f,  0.f,  1.f },
    glm::vec3{  0.f,  0.f,  1.f },
    glm::vec3{  0.f,  0.f,  1.f },
    glm::vec3{  0.f,  0.f,  1.f },
  },
  // Top
  {
    glm::vec3{  0.f,  1.f,  0.f },
    glm::vec3{  0.f,  1.f,  0.f },
    glm::vec3{  0.f,  1.f,  0.f },
    glm::vec3{  0.f,  1.f,  0.f },
  },
  // Bottom
  {
    glm::vec3{  0.f, -1.f,  0.f },
    glm::vec3{  0.f, -1.f,  0.f },
    glm::vec3{  0.f, -1.f,  0.f },
    glm::vec3{  0.f, -1.f,  0.f }
  }
};

static constexpr std::array<unsigned short, 6> CUBE_INDICES[] =
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

CBlockFactory::CBlockFactory()
{
  LoadConfigs();
}

CBlockFactory::~CBlockFactory() = default;

//
// Factory
//

CBlockFactory & CBlockFactory::Instance()
{
  static CBlockFactory Factory;

  return Factory;
}

//
// Service
//

TGLUnbakedMeshComponent CBlockFactory::GetMeshForBlock(const TBlockComponent & Block, EBlockFace Faces)
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

      Mesh.Normals.insert(Mesh.Normals.end(), CUBE_NORMALS[FaceIndex].begin(), CUBE_NORMALS[FaceIndex].end());
    }
  }

  return Mesh;
}

std::vector<glm::vec2> CBlockFactory::GetUVForBlock(const TBlockComponent & Block, EBlockFace Faces)
{
  std::vector<glm::vec2> UV;

  for (int FaceIndex = 0; FaceIndex < 6; ++FaceIndex)
  {
    if (Faces & (1 << FaceIndex))
    {
      const EBlockFace Face = static_cast<EBlockFace>(1 << FaceIndex);

      glm::vec2 UVMin = Instance().m_BlockUVs[Block.Type].Faces[FaceIndex];
      glm::vec2 UVMax = UVMin + Instance().m_BlockQuadSize;

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

bool CBlockFactory::IsBlockTransparent(const TBlockComponent & Block)
{
  return true;
}

void CBlockFactory::LoadConfigs()
{
  LoadBlockConfigs();
  LoadBlockUVs();
}

void CBlockFactory::LoadBlockConfigs()
{
  spdlog::info("Loading block configs on path {}", BLOCK_INFO_CONFIG_PATH);

  std::ifstream BlockInfoStream(BLOCK_INFO_CONFIG_PATH.data());

  if (!BlockInfoStream.is_open())
  {
    spdlog::critical("Failed to open block info config file");
    return;
  }

  const auto BlockInfoConfig = nlohmann::json::parse(BlockInfoStream);

  if (BlockInfoConfig.empty())
  {
    spdlog::critical("Failed to parse block info config file");
    return;
  }

  for (auto && BlockInfo : BlockInfoConfig)
  {
    TBlockInfo Info;

    BlockInfo["id"].get_to(Info.Type);
    BlockInfo["displayName"].get_to(Info.DisplayName);
    BlockInfo["name"].get_to(Info.Name);

    if (BlockInfo.contains("hardness") && !BlockInfo["hardness"].is_null())
      BlockInfo["hardness"].get_to(Info.Hardness);

    if (BlockInfo.contains("stackSize") && !BlockInfo["stackSize"].is_null())
      BlockInfo["stackSize"].get_to(Info.StackSize);

    BlockInfo["diggable"].get_to(Info.IsDiggable);
    BlockInfo["boundingBox"].get_to(Info.BoundingBoxType);
    // TODO: uncomment and implement BlockInfo["drops"].get_to(Info.DropIDs);
    BlockInfo["transparent"].get_to(Info.IsTransparent);
    BlockInfo["emitLight"].get_to(Info.EmitLight);
    BlockInfo["filterLight"].get_to(Info.FilterLight);
    BlockInfo["resistance"].get_to(Info.Resistance);

    m_BlockInfos.emplace(Info.Type, std::move(Info));
  }

  spdlog::info("Successfully loaded block info config file");
}

void CBlockFactory::LoadBlockUVs()
{
  spdlog::info("Loading block UVs on path {}", BLOCK_UV_CONFIG_PATH);

  std::ifstream BlockInfoStream(BLOCK_UV_CONFIG_PATH.data());

  if (!BlockInfoStream.is_open())
  {
    spdlog::critical("Failed to open block info config file");
    return;
  }

  const auto BlockUVsConfig = nlohmann::json::parse(BlockInfoStream);

  m_BlockAtlasID  = CTextureLibrary::Load(BlockUVsConfig["atlas"].get<std::string>()).TextureID;
  m_BlockShaderID = CShaderLibrary::Load("res/shaders/blocks_shader").ShaderID;

  auto && BlockAtlasSize = BlockUVsConfig["atlas_size"].get<std::pair<float, float>>();
  auto && BlockQuadSize  = BlockUVsConfig["quad_size"].get<std::pair<float, float>>();

  m_BlockAtlasSize = { BlockAtlasSize.first, BlockAtlasSize.second };

  m_BlockQuadSize = { BlockQuadSize.first, BlockQuadSize.second };
  m_BlockQuadSize /= m_BlockAtlasSize;

  for (auto && BlockUV : BlockUVsConfig["blocks"])
  {
    auto && UVConfig = BlockUV["uv"];

    TBlockUV UV{};

    for (size_t Index = 0; auto FaceID : {"front", "bottom", "left", "right", "top", "back"})
    {
      std::pair<float, float> FaceUV;

      if (UVConfig.contains(FaceID))
      {
        UVConfig[FaceID].get_to(FaceUV);
      }

      UV.Faces[Index++] = { FaceUV.first / m_BlockAtlasSize.x, FaceUV.second / m_BlockAtlasSize.y };
    }

    m_BlockUVs[static_cast<EBlockType>(BlockUV["id"].get<int>())] = UV;
  }

  spdlog::info("Successfully loaded block UV config file");
}