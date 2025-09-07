#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <stb/stb_image.h>
#include <map>
#include <stack>

#include "block.h"

class Textures
{
public:
    std::map<std::string, unsigned int> texture_map;
    


    void load_textures();
};