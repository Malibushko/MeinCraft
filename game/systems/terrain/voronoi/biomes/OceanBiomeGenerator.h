#pragma once
#include "game/systems/terrain/voronoi/BiomeGenerator.h"

class COceanBiomeGenerator : public CBiomeGenerator
{
public: // IBiomeGenerator

  using CBiomeGenerator::CBiomeGenerator;

  EBlockType GetBlockAt(int Elevation, int X_, int Y_) override;
};
