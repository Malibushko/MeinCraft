#include "FileUtils.h"

#include <filesystem>
#include <fstream>
#include <spdlog/spdlog.h>

std::string Utils::ReadFile(std::string_view Path)
{
  std::ifstream Stream(std::filesystem::current_path() / Path);

  if (!Stream.is_open())
  {
    spdlog::error("Unable to open file: {}", Path);

    return {};
  }

  std::string Data;

  Data.resize(std::filesystem::file_size(Path));

  Stream.read(Data.data(), Data.capacity());

  return Data;
}
