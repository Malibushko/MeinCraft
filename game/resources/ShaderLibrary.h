#pragma once
#include <map>
#include <entt/resource/cache.hpp>
#include <string_view>
#include "game/components/render/GLShaderComponent.h"

class CShaderLibrary
{
public: // Public structs

  struct CShaderLoader
  {
    using result_type = TGLShaderComponent;

    result_type operator()(std::string_view ShaderPath) const;
  };

public: // Interface

  static TGLShaderComponent Load(std::string_view ShaderPath);

protected: // Members

  std::map<entt::id_type, TGLShaderComponent> m_ShaderCache;
  CShaderLoader m_ShaderLoader;
};
