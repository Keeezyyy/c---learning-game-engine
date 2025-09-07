

#include "world.h"

World::World()
{
    // Beispiel: ein paar Blöcke initialisieren


}

void World::load_vertecies()
{
    int index = 0; // Zeiger für vertecies

    int numBlocks = BlocksToRender.size();

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
