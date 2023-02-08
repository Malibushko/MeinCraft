#pragma  once
#include <map>
#include <string_view>
#include <entt/resource/cache.hpp>

class CTextureLibrary
{
public: // Public structs

  struct TTexture
  {
    unsigned int TextureID;
  };

  struct CTextureLoader
  {
    using result_type = TTexture;

    result_type operator()(std::string_view TexturePath) const;
  };

public: // Interface

  static TTexture Load(std::string_view TexturePath);

protected: // Members

  std::map<entt::id_type, TTexture> m_TexturesCache;
  CTextureLoader m_TexturesLoader;
};
