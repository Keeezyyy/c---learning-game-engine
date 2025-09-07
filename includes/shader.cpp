#include <iostream>

#include "shader.h"
#include "utils.h"

Shader::Shader()
{
    VertexShaderSource = readFile("shader/vertexShader.vert");
    FragmentShaderSource = readFile("shader/vertexShader.frag");

    if (!VertexShaderSource || !FragmentShaderSource)
    {
        std::cout << "Shader files not found!" << std::endl;
        return;
    }
}