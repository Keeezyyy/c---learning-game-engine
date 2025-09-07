#include <iostream>
#include <map>
#include <string>

#include "block.h"
#include "../constants/texture_constants.h"


Block::Block(BlockType t, int x, int y, int z, std::map<std::string, unsigned int> texture_map)
{
    wordPos.x = x;
    wordPos.y = y;
    wordPos.z = z;
    type = t;

    std::array<std::string, 6> fileNames = texture_filename_map.at(t);



    for (int i = 0; i < 6; i++)
    {
        faces[i].textureID = texture_map[fileNames[i]];

        switch (i)
        {
        case (int)BlockFace::Top:
            faces[i].vertices[0] = {0, 1, 0, 0, 0};
            faces[i].vertices[1] = {1, 1, 0, 1, 0};
            faces[i].vertices[2] = {1, 1, 1, 1, 1};
            faces[i].vertices[3] = {0, 1, 0, 0, 0};
            faces[i].vertices[4] = {1, 1, 1, 1, 1};
            faces[i].vertices[5] = {0, 1, 1, 0, 1};
            break;

        case (int)BlockFace::Bottom:
            faces[i].vertices[0] = {0, 0, 0, 0, 0};
            faces[i].vertices[1] = {1, 0, 1, 1, 1};
            faces[i].vertices[2] = {1, 0, 0, 1, 0};
            faces[i].vertices[3] = {0, 0, 0, 0, 0};
            faces[i].vertices[4] = {0, 0, 1, 0, 1};
            faces[i].vertices[5] = {1, 0, 1, 1, 1};
            break;

        case (int)BlockFace::Left:
            faces[i].vertices[0] = {0, 0, 0, 0, 0};
            faces[i].vertices[1] = {0, 1, 1, 1, 1};
            faces[i].vertices[2] = {0, 1, 0, 1, 0};
            faces[i].vertices[3] = {0, 0, 0, 0, 0};
            faces[i].vertices[4] = {0, 0, 1, 0, 1};
            faces[i].vertices[5] = {0, 1, 1, 1, 1};
            break;

        case (int)BlockFace::Right:
            faces[i].vertices[0] = {1, 0, 0, 0, 0};
            faces[i].vertices[1] = {1, 1, 0, 1, 0};
            faces[i].vertices[2] = {1, 1, 1, 1, 1};
            faces[i].vertices[3] = {1, 0, 0, 0, 0};
            faces[i].vertices[4] = {1, 1, 1, 1, 1};
            faces[i].vertices[5] = {1, 0, 1, 0, 1};
            break;

        case (int)BlockFace::Front:
            faces[i].vertices[0] = {0, 0, 1, 0, 0};
            faces[i].vertices[1] = {1, 1, 1, 1, 1};
            faces[i].vertices[2] = {1, 0, 1, 1, 0};
            faces[i].vertices[3] = {0, 0, 1, 0, 0};
            faces[i].vertices[4] = {0, 1, 1, 0, 1};
            faces[i].vertices[5] = {1, 1, 1, 1, 1};
            break;

        case (int)BlockFace::Back:
            faces[i].vertices[0] = {0, 0, 0, 0, 0};
            faces[i].vertices[1] = {1, 0, 0, 1, 0};
            faces[i].vertices[2] = {1, 1, 0, 1, 1};
            faces[i].vertices[3] = {0, 0, 0, 0, 0};
            faces[i].vertices[4] = {1, 1, 0, 1, 1};
            faces[i].vertices[5] = {0, 1, 0, 0, 1};
            break;
        }

        // Nach Erzeugung: UVs rotieren
        if (i == (int)BlockFace::Front || i == (int)BlockFace::Back)
        {
            rotateUVRight(faces[i]);
            rotateUVRight(faces[i]);
        }
        if (i == (int)BlockFace::Right || i == (int)BlockFace::Left)
        {
            rotateUVRight(faces[i]);
        }
    }
};

void Block::rotateUVRight(BlockFaceData &face)
{
    for (int v = 0; v < 6; v++)
    {
        float oldU = face.vertices[v].u;
        float oldV = face.vertices[v].v;
        face.vertices[v].u = oldV;
        face.vertices[v].v = 1.0f - oldU;
    }
}
