#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>

int screenwidth = 640;
int screenheight = 480;

float vertecies[] =
    {
        0.5f,
        0.7f,
        0.0f,
        0.1f,
        -0.7f,
        0.0f,
        0.9f,
        -0.7f,
        0.0f,
};

char *readFile(const char *filename)
{

    FILE *file = fopen(filename, "rb");

    if (!file)
    {
        std::cout << "error";
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    rewind(file);

    // Speicher für Text + Nullterminator
    char *buffer = (char *)malloc(length + 1);
    if (!buffer)
    {
        fprintf(stderr, "Speicher konnte nicht reserviert werden\n");
        fclose(file);
        return NULL;
    }

    // Inhalt lesen
    size_t readSize = fread(buffer, 1, length, file);
    buffer[readSize] = '\0'; // Nullterminator anhängen

    fclose(file);
    return buffer;
}

int main()
{
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(screenwidth, screenheight, "testing", NULL, NULL);

    if (!window)
    {
        std::cout << " error creating window";
        return -1;
    }

    glfwMakeContextCurrent(window);
    gladLoadGL();
    glViewport(0, 0, screenwidth, screenheight);

    // Reading the shader files
    char *vertexShaderSource = readFile("shader/vertexShader.vert"), *FragmentShaderSource = readFile("shader/vertexShader.frag");

    // start width Buffer connections

    // VAO init
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);

    // VBO init
    GLuint VBO;

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertecies), vertecies, GL_STATIC_DRAW);
    // Initiating shaders

    // Vertex Shader
    GLuint vertextShader;
    vertextShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertextShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertextShader);

    // Fragment SHader
    GLuint fragShader;
    fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragShader, 1, &FragmentShaderSource, NULL);
    glCompileShader(fragShader);

    // shader Program init
    GLuint shaderProgram;
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertextShader);
    glAttachShader(shaderProgram, fragShader);
    glLinkProgram(shaderProgram);

    // Vertext interpretation
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDetachShader(shaderProgram, vertextShader);
    glDetachShader(shaderProgram, fragShader);

    glfwDestroyWindow(window);
    glfwTerminate();
}