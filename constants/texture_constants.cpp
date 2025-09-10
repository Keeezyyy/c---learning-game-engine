#include "texture_constants.h"

std::map<BlockType, std::array<std::string, 6>> texture_filename_map = {
    {BlockType::DIRT, {"dirt.png", "dirt.png", "dirt.png","dirt.png", "dirt.png", "dirt.png"}},
    {BlockType::GRASS, {"grass_top.png", "grass_top.png", "grass_side.png","grass_side.png", "grass_side.png", "grass_side.png"}},
    {BlockType::SPRUCE_LOG, {"log_spruce_top.png", "log_spruce_top.png", "log_spruce.png","log_spruce.png", "log_spruce.png", "log_spruce.png"}},
};

