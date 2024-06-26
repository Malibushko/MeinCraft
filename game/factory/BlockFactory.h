#pragma once
#include <map>

#include "core/entity/Entity.h"
#include "core/entity/Registry.h"
#include "game/components/lightning/LightComponent.h"
#include "game/components/lightning/PointLightComponent.h"
#include "game/components/render/GLShaderComponent.h"
#include "game/components/render/GLTextureComponent.h"
#include "game/components/render/GLUnbakedMeshComponent.h"
#include "game/components/render/MaterialComponent.h"
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

  static TGLUnbakedSolidMeshComponent GetCubeMeshForBlock(const TBlockComponent & Block, EBlockFace Faces);

  static TGLUnbakedSolidMeshComponent GetCrossMeshForBlock(const TBlockComponent & Block, EBlockFace Faces);

  static std::vector<glm::vec2> GetCubeUVForBlock(const TBlockComponent & Block, EBlockFace Faces);

  static bool IsBlockTranslucent(const TBlockComponent & Block);

  static bool IsBlockTransparent(const TBlockComponent & Block);

  static EMeshType GetMeshTypeForBlock(const TBlockComponent & Block);

  static TGLShaderComponent GetShaderForBlock(const TBlockComponent & Block);

  static EBlockFace GetDefaultBlockMeshFaces(const TBlockComponent & Block);

  static std::string GetIconPath(const TBlockComponent & Block);

  static TGLUnbakedSolidMeshComponent CreateRawCubeMesh();

  static int GetBlockEmitLightFactor(TBlockComponent Block);

  static const std::vector<TMaterialComponent> & GetBlockMaterials();

  static TLightComponent GetBlockLightComponent(const TBlockComponent & Block);

  static TPointLightComponent GetBlockPointLightComponent(const TBlockComponent & Block);

protected: // Service

  void LoadConfigs();

  void LoadBlockConfigs();

  void LoadBlockMaterialsInfo();

  void LoadBlockMaterials();

protected: // Service Structs

  struct TBlockInfo
  {
    EBlockType           Type {EBlockType::Invalid};
    std::string          DisplayName;
    std::string          Name;
    float                Hardness{};
    int                  StackSize{};
    bool                 IsDiggable{};
    std::string          BoundingBoxType;
    std::vector<int>     DropIDs{};
    bool                 IsTranslucent{};
    bool                 IsTransparent{};
    int                  EmitLight{};
    int                  FilterLight{};
    float                Resistance{};
    TLightComponent      Light{};
    TPointLightComponent PointLight{};
  };

  enum class EBlockMeshType
  {
    Cube  = 0,
    Cross = 1
  };

  struct TBlockMaterialInfo
  {
    std::array<glm::vec2, 6> Faces;
    EBlockMeshType           MeshType;
    unsigned int             MaterialID;
  };

protected: // Members

  unsigned int m_BlockAtlasID;
  unsigned int m_BlockShaderID;

  glm::vec2 m_BlockAtlasSize;
  glm::vec2 m_BlockQuadSize;

  std::map<EBlockType, TBlockMaterialInfo> m_BlockUVs;
  std::map<EBlockType, TBlockInfo>         m_BlockInfos;
  std::vector<TMaterialComponent>          m_BlockMaterials;
};
