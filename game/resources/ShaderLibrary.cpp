#include "ShaderLibrary.h"

#include <filesystem>
#include <entt/core/hashed_string.hpp>
#include <entt/resource/resource.hpp>
#include <glad/glad.h>
#include <spdlog/spdlog.h>

#include "game/utils/FileUtils.h"

static constexpr std::string_view VertexShaderExtention   = ".vert";
static constexpr std::string_view FragmentShaderExtention = ".frag";

bool CheckShaderCompilationErrors(int ShaderID)
{
  GLint success;
  char Info[512];
  glGetShaderiv(ShaderID, GL_COMPILE_STATUS, &success);

  if (!success)
  {
    glGetShaderInfoLog(ShaderID, sizeof(Info), nullptr, Info);

    spdlog::error("Shader compilation failed: {}", Info);
    return false;
  }

  return true;
}

bool CheckProgramLinkErrors(int ProgramID)
{
  GLint success;
  char Info[512];

  glGetProgramiv(ProgramID, GL_LINK_STATUS, &success);

  if (!success)
  {
    glGetProgramInfoLog(ProgramID, sizeof(Info), nullptr, Info);

    spdlog::error("Program link failed: {}", Info);

    return false;
  }

  return true;
}

//
// Public structs
//

CShaderLibrary::CShaderLoader::result_type CShaderLibrary::CShaderLoader::operator()(
    std::string_view ShaderPath
  ) const
{
  spdlog::info("Compiling shader: {}", ShaderPath);

  const std::string VertexShaderData = Utils::ReadFile(std::string(ShaderPath) + std::string(VertexShaderExtention));
  const std::string FragmentShaderData = Utils::ReadFile(std::string(ShaderPath) + std::string(FragmentShaderExtention));

  if (VertexShaderData.empty() || FragmentShaderData.empty())
    return {};

  const GLchar * VertexPtr = VertexShaderData.data(), * FragmentPtr = FragmentShaderData.data();

  const GLuint VertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(VertexShader, 1, &VertexPtr, nullptr);
  glCompileShader(VertexShader);

  const GLuint FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(FragmentShader, 1, &FragmentPtr, nullptr);
  glCompileShader(FragmentShader);

  if (!CheckShaderCompilationErrors(VertexShader) || !CheckShaderCompilationErrors(FragmentShader))
    return {};

  const unsigned int ShaderID = glCreateProgram();

  glAttachShader(ShaderID, VertexShader);
  glAttachShader(ShaderID, FragmentShader);

  glLinkProgram(ShaderID);

  glDeleteShader(VertexShader);
  glDeleteShader(FragmentShader);

  if (!CheckProgramLinkErrors(ShaderID))
    return {};

  spdlog::info("Shader compiled successfully: {}", ShaderPath);

  return TGLShaderComponent{ .ShaderID = ShaderID };
}

//
// Instance
//

static inline CShaderLibrary Library;

//
// Interface
//

TGLShaderComponent CShaderLibrary::Load(
    std::string_view ShaderPath
  )
{
  const entt::id_type ShaderID = entt::hashed_string(ShaderPath.data());

  if (const auto Iterator = Library.m_ShaderCache.find(ShaderID); Iterator != Library.m_ShaderCache.end())
    return Iterator->second;

  return Library.m_ShaderCache.emplace(ShaderID, Library.m_ShaderLoader(ShaderPath)).first->second;
}

std::vector<TGLShaderComponent> CShaderLibrary::GetLoadedShaders()
{
  std::vector<TGLShaderComponent> Shaders;

  for (auto & [Hash, Shader] : Library.m_ShaderCache)
    Shaders.push_back(Shader);

  return Shaders;
}
