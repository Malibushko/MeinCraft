#pragma once
#include "game/systems/terrain/voronoi/BiomeGenerator.h"

class CForestBiomeGenerator : public CBiomeGenerator
{
public: // IBiomeGenerator

  using CBiomeGenerator::CBiomeGenerator;

  EBlockType GetBlockAt(int BaseHeight, int Elevation, int X_, int Y_) override;
};
