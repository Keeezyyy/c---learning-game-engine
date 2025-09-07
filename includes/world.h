#pragma once

#include "block.h"
#include <vector>

class World
{

public:
    std::vector<Block> BlocksToRender;
    std::vector<float> vertecies;

    World(); // Konstruktor

    void load_vertecies();

    void add_block(BlockType t, int x, int y, int z, std::map<std::string, unsigned int>);
};