#pragma once
#include <memory>

#include "BiomeConfig.h"
#include "BiomeGenerator.h"
#include "game/components/terrain/BlockComponent.h"

class CBiomeGeneratorFactory
{
public: // Construction/Destruction

  CBiomeGeneratorFactory() = default;

  ~CBiomeGeneratorFactory() = default;

public: // Interface

  static std::unique_ptr<CBiomeGenerator> Create(const TBiomeConfig & Config);
};
