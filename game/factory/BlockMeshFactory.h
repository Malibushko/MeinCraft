#pragma once
#include "core/entity/Entity.h"
#include "core/entity/Registry.h"
#include "game/components/render/GLShaderComponent.h"
#include "game/components/render/GLTextureComponent.h"
#include "game/components/render/GLUnbakedMeshComponent.h"
#include "game/components/terrain/VisibleBlockFacesComponent.h"

struct TBlockComponent;

class CBlockMeshFactory
{
public: // Construction/Destruction

  CBlockMeshFactory();

  ~CBlockMeshFactory();

public: // Interface

  void InitBlock(registry_t & Registry, const entity_t & BlockEntity) const;

public: // Service

  TGLUnbakedMeshComponent GetMeshForBlock(const TBlockComponent & Block, EBlockFace Faces) const;

  TGLTextureComponent GetTextureForBlock(const TBlockComponent & Block) const;

  TGLShaderComponent GetShaderForBlock(const TBlockComponent & Block) const;

  std::vector<glm::vec2> GetUVForBlock(const TBlockComponent & Block, EBlockFace Faces) const;
};