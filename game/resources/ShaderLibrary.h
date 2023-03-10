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

    enum class EShaderType
    {
      Vertex,
      Fragment,
      Compute
    };

    result_type operator()(EShaderType ShaderType, std::string_view ShaderPath) const;

    result_type operator()(std::string_view ShaderPath) const;
  };

public: // Interface

  static TGLShaderComponent Load(std::string_view ShaderPath);

  static std::vector<TGLShaderComponent> GetLoadedShaders();

protected: // Members

  std::map<entt::id_type, TGLShaderComponent> m_ShaderCache;
  CShaderLoader m_ShaderLoader;
};
