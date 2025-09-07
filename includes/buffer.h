#pragma once

class Buffer
{
private:
    /* data */
public:
    unsigned int VAO;
    unsigned int VBO;
    Buffer();


    void VBOgen(int size, float vertecies[]);
    void VertexInterpretation();
    void update(int size, float *vertecies);
    ~Buffer();
};
