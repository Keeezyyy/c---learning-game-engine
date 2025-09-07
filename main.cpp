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
#include <glm/gtc/matrix_transform.hpp> // für lookAt(), perspective()
#include <glm/gtc/type_ptr.hpp>         // für value_ptr()
#include <dirent.h>
#include <vector>
#include <string>

//custom includes
#include "includes/utils.h"
#include "includes/shader.h"
#include "includes/textures.h"
#include "includes/block.h"
#include "includes/world.h"

int screenwidth = 640;
int screenheight = 480;


// 4. BlockFaceData speichert 6 Vertices pro Seite



struct vec3
{
    float x, y, z;

    // Addition
    vec3 operator+(const vec3 &other) const
    {
        return {x + other.x, y + other.y, z + other.z};
    }

    // Subtraktion
    vec3 operator-(const vec3 &other) const
    {
        return {x - other.x, y - other.y, z - other.z};
    }

    // Skalierung mit float
    vec3 operator*(float scalar) const
    {
        return {x * scalar, y * scalar, z * scalar};
    }

    // In-place Addition
    vec3 &operator+=(const vec3 &other)
    {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }

    // In-place Subtraktion
    vec3 &operator-=(const vec3 &other)
    {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        return *this;
    }
};

class Camera
{
public:
    vec3 cameraPos;
    vec3 U; // Right
    vec3 N; // Up
    vec3 V; // Forward

    Camera()
        : cameraPos{0.0f, 5.0f, 10.0f},
          U{1.0f, 0.0f, 0.0f},
          N{0.0f, 1.0f, 0.0f},
          V{0.0f, 0.0f, 1.0f}
    {
    }

    void moveForward(float delta) { cameraPos += V * delta; }
    void moveBackward(float delta) { cameraPos -= V * delta; }
    void moveRight(float delta) { cameraPos += U * delta; }
    void moveLeft(float delta) { cameraPos -= U * delta; }
    void moveUp(float delta) { cameraPos += N * delta; }
    void moveDown(float delta) { cameraPos -= N * delta; }

    void printPosition()
    {
        std::cout << "Camera Position: ("
                  << cameraPos.x << ", "
                  << cameraPos.y << ", "
                  << cameraPos.z << ")\n";
    }
};

float lastX = screenwidth / 2.0f;
float lastY = screenheight / 2.0f;
bool firstMouse = true;

float yaw = -90.0f; // Initial Richtung entlang -Z
float pitch = 0.0f;

Camera playerCam;


float deltaTime = 0.0f;

void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
    static double lastX = screenwidth / 2.0;
    static double lastY = screenheight / 2.0;
    static bool firstMouse = true;

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    double xoffset = xpos - lastX;
    double yoffset = lastY - ypos; // invertieren

    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    // yaw/pitch anpassen
    yaw += xoffset;
    pitch += yoffset;


    printf("\n");
    printf("yaw : %f", yaw);
    printf("pitch : %f", pitch);
    printf("\n");



    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

    playerCam.V = {direction.x, direction.y, direction.z};

    glm::vec3 worldUp = {0.0f, 1.0f, 0.0f};
    glm::vec3 forward(direction.x, direction.y, direction.z);
    glm::vec3 right = glm::normalize(glm::cross(forward, worldUp));
    glm::vec3 up = glm::normalize(glm::cross(right, forward));

    playerCam.U = {right.x, right.y, right.z};
    playerCam.N = {up.x, up.y, up.z};
}

glm::vec3 velocity(0.0f); // global oder als Member der Kamera

bool keys[1024] = {false};

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS)
            keys[key] = true;
        else if (action == GLFW_RELEASE)
            keys[key] = false;
    }
}

void processInput(float deltaTime)
{
    float moveSpeed = 5.0f;

    glm::vec3 direction(0.0f);

    if (keys[GLFW_KEY_W])
        direction += glm::vec3(playerCam.V.x, playerCam.V.y, playerCam.V.z);
    if (keys[GLFW_KEY_S])
        direction -= glm::vec3(playerCam.V.x, playerCam.V.y, playerCam.V.z);
    if (keys[GLFW_KEY_A])
        direction -= glm::vec3(playerCam.U.x, playerCam.U.y, playerCam.U.z);
    if (keys[GLFW_KEY_D])
        direction += glm::vec3(playerCam.U.x, playerCam.U.y, playerCam.U.z);

    if (glm::length(direction) > 0.0f)
        direction = glm::normalize(direction);

    playerCam.cameraPos.x += direction.x * moveSpeed * deltaTime;
    playerCam.cameraPos.y += direction.y * moveSpeed * deltaTime;
    playerCam.cameraPos.z += direction.z * moveSpeed * deltaTime;
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

    glEnable(GL_DEPTH_TEST); // Depth-Buffer aktivieren
    glDepthFunc(GL_LESS);

    // Reading the shader files
    Shader MyShaders;

    //Textures
    Textures MyTextures;

    MyTextures.load_textures();
    // Game initialisation

    World MyWorld;

    MyWorld.add_block(BlockType::DIRT, 0,0,0, MyTextures.texture_map);
    MyWorld.add_block(BlockType::DIRT, 0,0,1, MyTextures.texture_map);
    MyWorld.add_block(BlockType::DIRT, 0,0,2, MyTextures.texture_map);
    MyWorld.add_block(BlockType::DIRT, 0,0,3, MyTextures.texture_map);

    MyWorld.load_vertecies();
    
    printf("[Building World]size of vertecies %zu", MyWorld.vertecies.size());

    // VAO init
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);

    // VBO init
    GLuint VBO;

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, MyWorld.vertecies.size() * sizeof(float), MyWorld.vertecies.data(), GL_STATIC_DRAW);
    // Initiating shaders

    // Vertex Shader
    GLuint vertextShader;
    vertextShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertextShader, 1, &MyShaders.VertexShaderSource, NULL);
    glCompileShader(vertextShader);

    // Check vertex shader compilation
    int success;
    char infoLog[512];
    glGetShaderiv(vertextShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertextShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }

    // Fragment SHader
    GLuint fragShader;
    fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragShader, 1, &MyShaders.FragmentShaderSource, NULL);
    glCompileShader(fragShader);

    // Check fragment shader compilation
    glGetShaderiv(fragShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }

    // shader Program init
    GLuint shaderProgram;
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertextShader);
    glAttachShader(shaderProgram, fragShader);
    glLinkProgram(shaderProgram);

    // Check shader program linking
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
                  << infoLog << std::endl;
    }

    // Vertext interpretation
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void *)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(3);

    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // Maus verstecken & einfangen

    glfwSetKeyCallback(window, key_callback);

    float lastFrame = 0.0f; // vor dem Loop initialisieren

    free(MyShaders.VertexShaderSource);
    free(MyShaders.FragmentShaderSource);
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glfwPollEvents();
        processInput(deltaTime);

        glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glClear(GL_COLOR_BUFFER_BIT);

        // --- Kamera / View-Matrix ---
        glm::vec3 camPos(playerCam.cameraPos.x, playerCam.cameraPos.y, playerCam.cameraPos.z);
        glm::vec3 camForward(playerCam.V.x, playerCam.V.y, playerCam.V.z);
        glm::vec3 camUp(playerCam.N.x, playerCam.N.y, playerCam.N.z);
        glm::mat4 view = glm::lookAt(camPos, camPos + camForward, camUp);

        // --- Projektionsmatrix ---
        glm::mat4 projection = glm::perspective(
            glm::radians(45.0f),
            (float)screenwidth / (float)screenheight,
            0.1f, 100.0f);

        // --- Modellmatrix ---
        glm::mat4 model = glm::mat4(1.0f); // Identity-Matrix, kann für Animation/Transformation genutzt werden

        glUseProgram(shaderProgram);

        int samplers[16];
        for (int i = 0; i < 16; i++)
            samplers[i] = i; // [0,1,2,...,15]
        glUniform1iv(glGetUniformLocation(shaderProgram, "ourTexture"), 16, samplers);

        // Matrizen an den Shader senden
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, MyWorld.vertecies.size());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDetachShader(shaderProgram, vertextShader);
    glDetachShader(shaderProgram, fragShader);

    glfwDestroyWindow(window);
    glfwTerminate();
}