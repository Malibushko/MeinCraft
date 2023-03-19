#pragma once
#include "game/systems/terrain/voronoi/BiomeGenerator.h"

class CDesertBiomeGenerator : public CBiomeGenerator
{
public: // IBiomeGenerator

  using CBiomeGenerator::CBiomeGenerator;

  EBlockType GetBlockAt(int Elevation, int X_, int Y_) override;
};
