#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <stb/stb_image.h>
#include <map>
#include <stack>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <dirent.h>
#include <vector>
#include <string>

#include "textures.h"
#include "utils.h"

void Textures::load_textures()
{
    std::cout << "loading textures ... \n";
    std::string path = "./textures";

    DIR *dir = opendir(path.c_str());
    if (!dir)
    {
        std::cerr << "Ordner nicht gefunden!\n";
        return;
    }

    int idx = 0;
    struct dirent *entry;
    while ((entry = readdir(dir)) != nullptr)
    {
        if (entry->d_type == DT_REG)
        {
            std::string fileName = entry->d_name;
            std::string fullPath = path + "/" + fileName;

            unsigned int texID;

            glGenTextures(1, &texID);
            glActiveTexture(GL_TEXTURE0 + idx);
            glBindTexture(GL_TEXTURE_2D, texID);

            printf("loading texture : %s for the index: %u\n", entry->d_name, texID);

            // Scharfe Pixel (keine Interpolation)
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

            // Wiederholen in beide Richtungen (S = X, T = Y)
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

            int width, height, nrChannels;
            unsigned char *data = readPng(fullPath.c_str(), &width, &height, &nrChannels);

            if (data)
            {
                GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
                glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
                glGenerateMipmap(GL_TEXTURE_2D);

                texture_map[fileName] = idx;
            }
            else
            {
                std::cerr << "Failed to load texture: " << fileName << "\n";
            }

            stbi_image_free(data);
            idx++;
        }
    }
    closedir(dir);
    std::cout << "[finished] loading textures \n";
}