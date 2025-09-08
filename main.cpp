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
#include <thread> // for std::this_thread::sleep_for
#include <chrono> // for std::chrono::seconds
#include <optional>
// custom includes
#include "includes/utils.h"
#include "includes/shader.h"
#include "includes/textures.h"
#include "includes/block.h"
#include "includes/world.h"
#include "includes/buffer.h"
#include "includes/camera.h"

int screenwidth = 680;
int screenheight = 480;

// 4. BlockFaceData speichert 6 Vertices pro Seite

float deltaTime = 0.0f;

Camera PlayerCam(screenwidth, screenheight);

int main()
{
    glfwInit();

    PlayerCam.cameraPos.y = 10.0f;
    PlayerCam.cameraPos.x = 5.0f;
    PlayerCam.cameraPos.z = 5.0f;

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

    // Textures
    Textures MyTextures;

    MyTextures.load_textures();
    // Game initialisation

    World MyWorld;

    for (int x = 0; x < 20; x++)
    {
        for (int z = 0; z < 20; z++)
        {
            // Bodenschicht aus Dirt
            MyWorld.add_block(BlockType::DIRT, x, 0, z, MyTextures.texture_map);

            // Oberschicht aus Grass
            MyWorld.add_block(BlockType::GRASS, x, 1, z, MyTextures.texture_map);
        }
    }
    
    MyWorld.add_block(BlockType::DIRT, 5, 3, 5, MyTextures.texture_map);
    MyWorld.add_block(BlockType::DIRT, 6, 6, 7, MyTextures.texture_map);

    MyWorld.load_vertecies();

    printf("[Building World]size of vertecies %zu", MyWorld.vertecies.size());

    // VAO init
    Buffer MyBuffer;

    // Bind the vertecie data to VBO
    MyBuffer.VBOgen(MyWorld.vertecies.size() * sizeof(float), MyWorld.vertecies.data());

    Shader MyShader;

    MyShader.vertex_shader();
    MyShader.fragment_shader();
    MyShader.shader_program();

    // Vertex Interpretaion
    MyBuffer.VertexInterpretation();

    glfwSetWindowUserPointer(window, &PlayerCam); // Instanz speichern
    glfwSetCursorPosCallback(window, Camera::mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // Maus verstecken & einfangen

    glfwSetKeyCallback(window, Camera::key_callback);

    MyShader.~Shader();

    bool ePressedLastFrame = false;

    float lastFrame = 0.0f; // vor dem Loop initialisieren
    while (!glfwWindowShouldClose(window))
    {

        PlayerCam.blocks = MyWorld.BlocksToRender;

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glfwPollEvents();
        PlayerCam.processInput(deltaTime);
        PlayerCam.updatePhysics(deltaTime);

        // Nur bei Tastendruck Blöcke zu Grass ändern (z.B. mit linker Maustaste)
        bool ePressedThisFrame = glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS;

        PlayerCam.getNextBlockLookingAt();
        glm::vec3 tmp = PlayerCam.place_block();

        if (tmp.x != 0.989f && tmp.y != 0.989f && tmp.z != 0.989f)
        {
            MyWorld.add_block(BlockType::DIRT, tmp.x, tmp.y, tmp.z, MyTextures.texture_map);
        }

        MyWorld.load_vertecies();
        MyBuffer.VBOgen(MyWorld.vertecies.size() * sizeof(float), MyWorld.vertecies.data());
        MyBuffer.VertexInterpretation(); // nicht vergessen!

        ePressedLastFrame = ePressedThisFrame;
        glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glClear(GL_COLOR_BUFFER_BIT);

        // --- Kamera / View-Matrix ---
        glm::vec3 camPos(PlayerCam.cameraPos.x, PlayerCam.cameraPos.y, PlayerCam.cameraPos.z);
        glm::vec3 camForward(PlayerCam.V.x, PlayerCam.V.y, PlayerCam.V.z);
        glm::vec3 camUp(PlayerCam.N.x, PlayerCam.N.y, PlayerCam.N.z);
        glm::mat4 view = glm::lookAt(camPos, camPos + camForward, camUp);

        // --- Projektionsmatrix ---
        glm::mat4 projection = glm::perspective(
            glm::radians(45.0f),
            (float)screenwidth / (float)screenheight,
            0.1f, 100.0f);

        // --- Modellmatrix ---
        glm::mat4 model = glm::mat4(1.0f); // Identity-Matrix, kann für Animation/Transformation genutzt werden

        glUseProgram(MyShader.shaderProgram);

        int samplers[16];
        for (int i = 0; i < 16; i++)
            samplers[i] = i; // [0,1,2,...,15]
        glUniform1iv(glGetUniformLocation(MyShader.shaderProgram, "ourTexture"), 16, samplers);

        // Matrizen an den Shader senden
        glUniformMatrix4fv(glGetUniformLocation(MyShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(MyShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(MyShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        glBindVertexArray(MyBuffer.VAO);
        glDrawArrays(GL_TRIANGLES, 0, MyWorld.vertecies.size());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDetachShader(MyShader.shaderProgram, MyShader.vertextShader);
    glDetachShader(MyShader.shaderProgram, MyShader.fragmentShader);

    glfwDestroyWindow(window);
    glfwTerminate();
}