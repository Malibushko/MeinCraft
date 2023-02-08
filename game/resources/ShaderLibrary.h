#pragma once
#include <map>
#include <entt/resource/cache.hpp>
#include <string_view>

class CShaderLibrary
{
public: // Public structs

  struct TShader
  {
    unsigned int ShaderID;
  };

  struct CShaderLoader
  {
    using result_type = TShader;

    result_type operator()(std::string_view ShaderPath) const;
  };

public: // Interface

  static TShader Load(std::string_view ShaderPath);

protected: // Members

  std::map<entt::id_type, TShader> m_ShaderCache;
  CShaderLoader m_ShaderLoader;
};