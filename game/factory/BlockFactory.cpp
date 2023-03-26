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
#include "game/utils/FileUtils.h"


//
// Config
//

static constexpr std::string_view BLOCK_UV_CONFIG_PATH        = "res/configs/blocks_materials_data.json";
static constexpr std::string_view BLOCK_INFO_CONFIG_PATH      = "res/configs/blocks.json";
static constexpr std::string_view BLOCK_MATERIALS_CONFIG_PATH = "res/configs/block_materials.json";
static constexpr std::string_view BLOCK_ICONS_CACHE_PATH      = "res/icons/";

static constexpr std::array<glm::vec3, 4> CUBE_FACES[]
{
  {
    // Front
    glm::vec3(-0.5f, -0.5f,  0.5f),
    glm::vec3(0.5f, -0.5f,  0.5f),
    glm::vec3(0.5f,  0.5f,  0.5f),
    glm::vec3(-0.5f,  0.5f,  0.5f)
  },
  {
    // Back
    glm::vec3(-0.5f, -0.5f, -0.5f),
    glm::vec3(-0.5f,  0.5f, -0.5f),
    glm::vec3(0.5f,  0.5f, -0.5f),
    glm::vec3(0.5f, -0.5f, -0.5f)
  },
  // Left
  {
    glm::vec3(-0.5f, -0.5f, -0.5f),
    glm::vec3(-0.5f, -0.5f,  0.5f),
    glm::vec3(-0.5f,  0.5f,  0.5f),
    glm::vec3(-0.5f,  0.5f, -0.5f)
  },
  {
    // Right
    glm::vec3(0.5f, -0.5f, -0.5f),
    glm::vec3(0.5f,  0.5f, -0.5f),
    glm::vec3(0.5f,  0.5f,  0.5f),
    glm::vec3(0.5f, -0.5f,  0.5f)
  },
  {
    // Top
    glm::vec3(-0.5f,  0.5f, -0.5f),
    glm::vec3(-0.5f,  0.5f,  0.5f),
    glm::vec3(0.5f,  0.5f,  0.5f),
    glm::vec3(0.5f,  0.5f, -0.5f)
  },
  {
    // Bottom
    glm::vec3(-0.5f, -0.5f, -0.5f),
    glm::vec3(0.5f, -0.5f, -0.5f),
    glm::vec3(0.5f, -0.5f,  0.5f),
    glm::vec3(-0.5f, -0.5f,  0.5f)
  }
};

static constexpr std::array<glm::vec3, 4> CUBE_NORMALS[]  =
{
  // Front
  {
    glm::vec3{  0.f,  0.f,  1.f },
    glm::vec3{  0.f,  0.f,  1.f },
    glm::vec3{  0.f,  0.f,  1.f },
    glm::vec3{  0.f,  0.f,  1.f },
  },
  // Back
  {
    glm::vec3{ 0.f,  0.f,  -1.f },
    glm::vec3{ 0.f,  0.f,  -1.f },
    glm::vec3{ 0.f,  0.f,  -1.f },
    glm::vec3{ 0.f,  0.f,  -1.f },
  },
  // Left
  {
    glm::vec3{  -1.f,  0.f,  0.f },
    glm::vec3{  -1.f,  0.f,  0.f },
    glm::vec3{  -1.f,  0.f,  0.f },
    glm::vec3{  -1.f,  0.f,  0.f },
  },
  // Right
  {
    glm::vec3{  1.f,  0.f, 0.f},
    glm::vec3{  1.f,  0.f, 0.f},
    glm::vec3{  1.f,  0.f, 0.f},
    glm::vec3{  1.f,  0.f, 0.f},
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
  {0, 1, 2, 2, 3, 0},
  {4, 5, 6, 6, 7, 4},
  {8, 9, 10, 10, 11, 8},
  {12, 13, 14, 14, 15, 12},
  {16, 17, 18, 18, 19, 16},
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

TGLUnbakedSolidMeshComponent CBlockFactory::GetMeshForBlock(const TBlockComponent & Block, EBlockFace Faces)
{
  switch (Instance().m_BlockUVs[Block.Type].MeshType)
  {
    case EBlockMeshType::Cube:
      return GetCubeMeshForBlock(Block, Faces);
    case EBlockMeshType::Cross:
      return GetCrossMeshForBlock(Block, Faces);
  }

  assert(false);
  return TGLUnbakedSolidMeshComponent{};
}

TGLUnbakedSolidMeshComponent CBlockFactory::GetCubeMeshForBlock(const TBlockComponent & Block, EBlockFace Faces)
{
  TGLUnbakedSolidMeshComponent Mesh;

  int ActiveFaceIndex = 0;

  for (int FaceIndex = 0; FaceIndex < 6; ++FaceIndex)
  {
    if (Faces & (1 << FaceIndex))
    {
      Mesh.Vertices.insert(Mesh.Vertices.end(), CUBE_FACES[FaceIndex].begin(), CUBE_FACES[FaceIndex].end());
      Mesh.Indices.insert(Mesh.Indices.end(), CUBE_INDICES[ActiveFaceIndex].begin(), CUBE_INDICES[ActiveFaceIndex].end());

      ActiveFaceIndex++;

      Mesh.UV = GetCubeUVForBlock(Block, Faces);

      Mesh.Normals.insert(Mesh.Normals.end(), CUBE_NORMALS[FaceIndex].begin(), CUBE_NORMALS[FaceIndex].end());
    }
  }

  Mesh.MaterialID = Instance().m_BlockUVs[Block.Type].MaterialID;

  return Mesh;
}

TGLUnbakedSolidMeshComponent CBlockFactory::GetCrossMeshForBlock(const TBlockComponent & Block, EBlockFace Faces)
{
  TGLUnbakedSolidMeshComponent Mesh;

  static constexpr glm::vec3 FirstQuadVertices[] =
  {
    // Front face
    glm::vec3(-0.5f, -0.5f, 0.5f),
    glm::vec3(0.5f, -0.5f, -0.5f),
    glm::vec3(0.5f, 0.5f, -0.5f),
    glm::vec3(-0.5f, 0.5f, 0.5f)
  };

  static constexpr glm::vec3 SecondQuadVertices[] =
  {
    glm::vec3(-0.5f, -0.5f, -0.5f),
    glm::vec3(0.5f, -0.5f,  0.5f),
    glm::vec3(0.5f, 0.5f,   0.5f),
    glm::vec3(-0.5f, 0.5f, -0.5f)
  };

  static constexpr glm::vec3 FirstQuadNormals[] =
  {
    // Front face
    glm::vec3(1.0f, 0.5f, 1.0f),
    glm::vec3(1.0f, 0.5f, 1.0f),
    glm::vec3(1.0f, 0.5f, 1.0f),
    glm::vec3(1.0f, 0.5f, 1.0f),
  };

  static constexpr glm::vec3 FirstQuadInversedNormals[] =
  {
    // Front face
    glm::vec3(-1.0f, -0.5f, -1.0f),
    glm::vec3(-1.0f, -0.5f, -1.0f),
    glm::vec3(-1.0f, -0.5f, -1.0f),
    glm::vec3(-1.0f, -0.5f, -1.0f),
  };

  static constexpr glm::vec3 SecondQuadNormals[] =
  {
    glm::vec3(-1.0f, 0.5f, 1.0f),
    glm::vec3(-1.0f, 0.5f, 1.0f),
    glm::vec3(-1.0f, 0.5f, 1.0f),
    glm::vec3(-1.0f, 0.5f, 1.0f),
  };

  static constexpr glm::vec3 SecondQuadInversedNormals[] =
  {
    glm::vec3(1.0f, -0.5f, -1.0f),
    glm::vec3(1.0f, -0.5f, -1.0f),
    glm::vec3(1.0f, -0.5f, -1.0f),
    glm::vec3(1.0f, -0.5f, -1.0f),
  };

  static constexpr int QuadIndices[] =
  {
    0, 1, 2, 2, 3, 0,
    4, 6, 5, 7, 6, 4,
    8, 9, 10, 10, 11, 8,
    13, 12, 14, 15, 14, 12,
  };

  Mesh.Vertices.reserve(std::size(FirstQuadVertices) * 2  + std::size(SecondQuadVertices) * 2);
  Mesh.Vertices.insert(Mesh.Vertices.end(), std::begin(FirstQuadVertices), std::end(FirstQuadVertices));
  Mesh.Vertices.insert(Mesh.Vertices.end(), std::begin(FirstQuadVertices), std::end(FirstQuadVertices));
  Mesh.Vertices.insert(Mesh.Vertices.end(), std::begin(SecondQuadVertices), std::end(SecondQuadVertices));
  Mesh.Vertices.insert(Mesh.Vertices.end(), std::begin(SecondQuadVertices), std::end(SecondQuadVertices));

  Mesh.Indices.reserve(std::size(QuadIndices));
  Mesh.Indices.insert(Mesh.Indices.end(), std::begin(QuadIndices), std::end(QuadIndices));

  Mesh.Normals.reserve(std::size(FirstQuadNormals) + std::size(SecondQuadNormals));
  Mesh.Normals.insert(Mesh.Normals.end(), std::begin(FirstQuadNormals), std::end(FirstQuadNormals));
  Mesh.Normals.insert(Mesh.Normals.end(), std::begin(FirstQuadInversedNormals), std::end(FirstQuadInversedNormals));
  Mesh.Normals.insert(Mesh.Normals.end(), std::begin(SecondQuadNormals), std::end(SecondQuadNormals));
  Mesh.Normals.insert(Mesh.Normals.end(), std::begin(SecondQuadInversedNormals), std::end(SecondQuadInversedNormals));

  const auto UV = GetCubeUVForBlock(Block, EBlockFace::Front);

  Mesh.UV.reserve(UV.size() * 2);
  Mesh.UV.insert(Mesh.UV.end(), UV.begin(), UV.end());
  Mesh.UV.insert(Mesh.UV.end(), UV.begin(), UV.end());
  Mesh.UV.insert(Mesh.UV.end(), UV.begin(), UV.end());
  Mesh.UV.insert(Mesh.UV.end(), UV.begin(), UV.end());

  Mesh.MaterialID = Instance().m_BlockUVs[Block.Type].MaterialID;

  return Mesh;
}

std::vector<glm::vec2> CBlockFactory::GetCubeUVForBlock(const TBlockComponent & Block, EBlockFace Faces)
{
  std::vector<glm::vec2> UV;

  for (int FaceIndex = 0; FaceIndex < 6; ++FaceIndex)
  {
    if (Faces & (1 << FaceIndex))
    {
      const EBlockFace Face  = static_cast<EBlockFace>(1 << FaceIndex);
      const glm::vec2  UVMin = Instance().m_BlockUVs[Block.Type].Faces[FaceIndex];
      const glm::vec2  UVMax = UVMin + Instance().m_BlockQuadSize;

      switch (Face)
      {
        case EBlockFace::Right:
          UV.insert(UV.end(), { {UVMin.x, UVMax.y}, {UVMin.x, UVMin.y}, {UVMax.x, UVMin.y}, {UVMax.x, UVMax.y} });
          break;

        case EBlockFace::Left:
          UV.insert(UV.end(), { {UVMin.x, UVMax.y}, {UVMax.x, UVMax.y}, {UVMax.x, UVMin.y}, {UVMin.x, UVMin.y}});
          break;

        case EBlockFace::Top:
        case EBlockFace::Bottom:
          UV.insert(UV.end(), { {UVMax.x, UVMin.y}, {UVMin.x, UVMin.y}, {UVMin.x, UVMax.y}, {UVMax.x, UVMax.y}, });
          break;

        case EBlockFace::Front:
          UV.insert(UV.end(), { {UVMin.x, UVMax.y}, {UVMax.x, UVMax.y}, {UVMax.x, UVMin.y}, {UVMin.x, UVMin.y}, });
          break;

        case EBlockFace::Back:
          UV.insert(UV.end(), { {UVMax.x, UVMax.y}, {UVMax.x, UVMin.y}, {UVMin.x, UVMin.y}, {UVMin.x, UVMax.y}});
          break;
      }
    }
  }

  return UV;
}

bool CBlockFactory::IsBlockTranslucent(const TBlockComponent & Block)
{
  return Instance().m_BlockInfos[Block.Type].IsTranslucent;
}

bool CBlockFactory::IsBlockTransparent(const TBlockComponent & Block)
{
  return Instance().m_BlockInfos[Block.Type].IsTransparent;
}

EMeshType CBlockFactory::GetMeshTypeForBlock(const TBlockComponent & Block)
{
  if (IsBlockTranslucent(Block))
    return EMeshType::Translucent;

  return EMeshType::Solid;
}

TGLShaderComponent CBlockFactory::GetShaderForBlock(const TBlockComponent& Block)
{
  if (GetMeshTypeForBlock(Block) == EMeshType::Translucent)
  {
    return TGLShaderComponent
    {
      .ShaderID = CShaderLibrary::Load("res/shaders/transparent_blocks_shader").ShaderID
    };
  }

  return TGLShaderComponent
  {
    .ShaderID = CShaderLibrary::Load("res/shaders/blocks_shader").ShaderID
  };
}

EBlockFace CBlockFactory::GetDefaultBlockMeshFaces(const TBlockComponent & Block)
{
  // TODO: move to config
  if (Block.Type == EBlockType::Leaves)
    return EBlockFace::All;

  return EBlockFace::None;
}

std::string CBlockFactory::GetIconPath(const TBlockComponent & Block)
{
  return std::string(BLOCK_ICONS_CACHE_PATH) + std::to_string(static_cast<int>(Block.Type)) + ".png";
}

TGLUnbakedSolidMeshComponent CBlockFactory::CreateRawCubeMesh()
{
  TGLUnbakedSolidMeshComponent Mesh;

  for (int FaceIndex = 0; FaceIndex < 6; ++FaceIndex)
  {
    Mesh.Vertices.insert(Mesh.Vertices.end(), CUBE_FACES[FaceIndex].begin(),   CUBE_FACES[FaceIndex].end());
    Mesh.Indices .insert(Mesh.Indices.end(),  CUBE_INDICES[FaceIndex].begin(), CUBE_INDICES[FaceIndex].end());
    Mesh.Normals .insert(Mesh.Normals.end(),  CUBE_NORMALS[FaceIndex].begin(), CUBE_NORMALS[FaceIndex].end());
  }

  return Mesh;
}

int CBlockFactory::GetBlockEmitLightFactor(TBlockComponent Block)
{
  return Instance().m_BlockInfos[Block.Type].EmitLight;
}

const std::vector<TMaterialComponent> & CBlockFactory::GetBlockMaterials()
{
  return Instance().m_BlockMaterials;
}

void CBlockFactory::LoadConfigs()
{
  LoadBlockConfigs();
  LoadBlockUVs();
  LoadBlockMaterials();
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

    if (BlockInfo.contains("translucent") && !BlockInfo["translucent"].is_null())
      BlockInfo["translucent"].get_to(Info.IsTranslucent);

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

    TBlockMaterialInfo UV{};

    for (size_t Index = 0; auto FaceID : {"front", "back", "left", "right", "top", "bottom"})
    {
      std::pair<float, float> FaceUV;

      if (UVConfig.contains(FaceID))
      {
        UVConfig[FaceID].get_to(FaceUV);
      }

      UV.Faces[Index++] = { FaceUV.first / m_BlockAtlasSize.x, FaceUV.second / m_BlockAtlasSize.y };
    }

    UV.MeshType   = static_cast<EBlockMeshType>(BlockUV["mesh"].get<int>());
    UV.MaterialID = BlockUV["material_id"].get<int>();

    m_BlockUVs[static_cast<EBlockType>(BlockUV["id"].get<int>())] = UV;
  }

  spdlog::info("Successfully loaded block UV config file");
}

void CBlockFactory::LoadBlockMaterials()
{
  nlohmann::json Config = nlohmann::json::parse(Utils::ReadFile(BLOCK_MATERIALS_CONFIG_PATH));

  if (Config.empty())
  {
    spdlog::critical("Failed to load block materials config file");
    return;
  }

  for (const auto & MaterialConfig : Config)
  {
    TMaterialComponent Material;

    Material.Metallic  = MaterialConfig["metallic"];
    Material.Roughness = MaterialConfig["roughness"];
    Material.Emissive  = MaterialConfig["emissive"];

    if (MaterialConfig.find("emissive_color") != MaterialConfig.end())
    {
      const auto EmissiveColor = MaterialConfig["emissive_color"];

      for (int i = 0; i < 4; i++)
        Material.EmissiveColor[i] = EmissiveColor[i];
    }

    m_BlockMaterials.push_back(std::move(Material));
  }
}
