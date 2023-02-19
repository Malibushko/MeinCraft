#include "TextureLibrary.h"

#include <entt/core/hashed_string.hpp>
#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#include <spdlog/spdlog.h>

#include "stb_image.h"

CTextureLibrary::CTextureLoader::result_type CTextureLibrary::CTextureLoader::operator()(
      std::string_view TexturePath
   ) const
{
  int Width, Height, Channels;

  if (const auto Data = stbi_load(TexturePath.data(), &Width, &Height, &Channels, 0))
  {
    GLenum Format         = GL_RGB;
    GLenum InternalFormat = GL_RGB;

    if (Channels == 1)
    {
      Format = GL_RED;
    }
    else if (Channels == 3)
    {
      Format         = GL_RGB;
      InternalFormat = GL_RGB;
    }
    else if (Channels == 4)
    {
      Format         = GL_RGBA;
      InternalFormat = GL_RGBA;
    }

    GLuint Texture;

    glGenTextures(1, &Texture);
    glBindTexture(GL_TEXTURE_2D, Texture);
    glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, Width, Height, 0, Format, GL_UNSIGNED_BYTE, Data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    stbi_image_free(Data);

    spdlog::info("Texture loaded: {}", TexturePath);

    return TTexture{ .TextureID = Texture };
  }

  spdlog::error("Unable to load texture: {}", TexturePath);

  return {};
}

//
//
//

static inline CTextureLibrary Library;

//
// Interface
//

CTextureLibrary::TTexture CTextureLibrary::Load(std::string_view TexturePath)
{
  const entt::id_type TextureID = entt::hashed_string(TexturePath.data());

  if (const auto Iterator = Library.m_TexturesCache.find(TextureID);
      Iterator != Library.m_TexturesCache.end())
    return Iterator->second;

  return Library.m_TexturesCache.emplace(TextureID, Library.m_TexturesLoader(TexturePath)).first->second;
}
