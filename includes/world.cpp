

#include "world.h"
#include "block.h"

World::World()
{
    // Beispiel: ein paar Blöcke initialisieren
}

void World::load_vertecies()
{
    int index = 0; // Zeiger für vertecies

    int numBlocks = BlocksToRender.size();

    // Ensure output buffer is large enough: 6 faces * 6 vertices per face * 9 floats per vertex
    const int facesPerBlock = 6;
    const int verticesPerFace = 6;
    const int floatsPerVertex = 9; // x,y,z,u,v, texId, worldX, worldY, worldZ
    size_t requiredSize = static_cast<size_t>(numBlocks) * facesPerBlock * verticesPerFace * floatsPerVertex;
    vertecies.clear();
    vertecies.resize(requiredSize);

    for (int i = 0; i < numBlocks; i++)
    {
        Block b = BlocksToRender[i];

        for (int k = 0; k < 6; k++)
        {
            BlockFaceData face = b.faces[k];

            for (int j = 0; j < 6; j++)
            {
                Vertex v = face.vertices[j];

                // Vertex
                vertecies[index++] = v.x;
                vertecies[index++] = v.y;
                vertecies[index++] = v.z;
                vertecies[index++] = v.u;
                vertecies[index++] = v.v;

                // TextureId
                vertecies[index++] = face.textureID;

                // World Pos
                vertecies[index++] = (float)b.wordPos.x;
                vertecies[index++] = (float)b.wordPos.y;
                vertecies[index++] = (float)b.wordPos.z;
            }
        }
    }
}

void World::add_block(BlockType t, int x, int y, int z, std::map<std::string, unsigned int> map)
{
    Block newBlock(t, x, y, z, map);

    BlocksToRender.push_back(newBlock);
}

void World::remove_block(int x, int y, int z)
{

    printf("removing for x: %d y: %d, z: %d  \n", x, y, z);

    printf("removing blocks \n");

    int length = BlocksToRender.size();

    std::vector<Block> tmp;

    for (int i = 0; i < length; i++)
    {
        Block b = BlocksToRender[i];

        if (b.wordPos.x == x &&
            b.wordPos.y == y &&
            b.wordPos.z == z)
        {
        }
        else
        {
            tmp.push_back(b);
        }
    }

    BlocksToRender = tmp;


    World::load_vertecies();
}