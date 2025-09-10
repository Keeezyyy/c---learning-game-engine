#pragma once

#include <iostream>
#include <map>
#include <string>

enum class BlockType
{
    AIR,
    GRASS,
    DIRT,
    STONE,
    SPRUCE_LOG
};

// 2. Blockfaces
enum class BlockFace
{
    Top,
    Bottom,
    Left,
    Right,
    Front,
    Back
};

// 3. Struktur für einen Vertex
struct Vertex
{
    float x, y, z; // Position
    float u, v;    // Texturkoordinaten
};

struct BlockFaceData
{
    Vertex vertices[6]; // 2 Dreiecke = 6 Vertices
    unsigned int textureID;
};

struct vector3Int
{
    int x, y, z;
};

class Block
{
public:
    BlockType type;
    BlockFaceData faces[6]; // Top, Bottom, Left, Right, Front, Back
    vector3Int wordPos;

    Block(BlockType t, int x, int y, int z, std::map<std::string, unsigned int>);

    void rotateUVRight(BlockFaceData &face);

};