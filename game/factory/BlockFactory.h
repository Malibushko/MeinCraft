#pragma once
#include <map>

#include "core/entity/Entity.h"
#include "core/entity/Registry.h"
#include "game/components/render/GLShaderComponent.h"
#include "game/components/render/GLTextureComponent.h"
#include "game/components/render/GLUnbakedMeshComponent.h"
#include "game/components/terrain/BlockComponent.h"
#include "game/components/terrain/VisibleBlockFacesComponent.h"

struct TBlockComponent;

class CBlockFactory
{
protected: // Construction/Destruction

  CBlockFactory();

  ~CBlockFactory();

protected: // Factory

  static CBlockFactory & Instance();

public: // Interface

  static TGLUnbakedSolidMeshComponent GetMeshForBlock(const TBlockComponent & Block, EBlockFace Faces);

  static std::vector<glm::vec2> GetUVForBlock(const TBlockComponent & Block, EBlockFace Faces);

  static bool IsBlockTransparent(const TBlockComponent & Block);

  static EMeshType GetMeshTypeForBlock(const TBlockComponent & Block);

  static TGLShaderComponent GetShaderForBlock(const TBlockComponent & Block);

protected: // Service

  void LoadConfigs();

  void LoadBlockConfigs();

  void LoadBlockUVs();

protected: // Service Structs

  struct TBlockInfo
  {
    EBlockType       Type {EBlockType::Invalid};
    std::string      DisplayName;
    std::string      Name;
    float            Hardness{};
    int              StackSize{};
    bool             IsDiggable{};
    std::string      BoundingBoxType;
    std::vector<int> DropIDs{};
    bool             IsTransparent{};
    int              EmitLight{};
    int              FilterLight{};
    float            Resistance{};
  };

  struct TBlockUV
  {
    std::array<glm::vec2, 6> Faces;
  };

protected: // Members

  unsigned int m_BlockAtlasID;
  unsigned int m_BlockShaderID;

  glm::vec2 m_BlockAtlasSize;
  glm::vec2 m_BlockQuadSize;

  std::map<EBlockType, TBlockUV>   m_BlockUVs;
  std::map<EBlockType, TBlockInfo> m_BlockInfos;

};
