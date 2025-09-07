
#pragma once


class Shader
{
public:
    char *VertexShaderSource;
    char *FragmentShaderSource;
    
    unsigned int vertextShader;
    unsigned int fragmentShader;
    unsigned int shaderProgram;

    int success;
    char infoLog[512];

    Shader();
    ~Shader();

    void vertex_shader();
    void fragment_shader();
    void shader_program();
};