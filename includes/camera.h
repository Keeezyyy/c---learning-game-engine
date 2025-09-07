#pragma once
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

#include "block.h"
#include "utils.h"
#include "world.h"

class Camera
{
private:
    /* data */
public:
    glm::vec3 velocity(); // global oder als Member der Kamera

    bool keys[1024] = {false};

    bool firstMouse = true;

    float yaw = 90.0f; // Initial Richtung entlang -Z
    float pitch = 0.0f;

    float lastX;
    float lastY;

    int screenwidth;
    int screenheight;

    glm::vec3 cameraPos;
    glm::vec3 U; // Right
    glm::vec3 N; // Up
    glm::vec3 V; // Forward


    float sizeDown = 1.7f;


    float JumpVelocity = 0.0f;
    bool isJumping = false;

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

    Camera(int screenwidth, int screenheight);

    static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);

    // Instanz-Funktion, die die Tasten verarbeitet
    void handleKey(int key, int scancode, int action, int mods);

    void processInput(float deltaTime);
    static void mouse_callback(GLFWwindow *window, double xpos, double ypos);
    void handleMouse(double xpos, double ypos);

    float getGroundHeight(const std::vector<Block> &blocks);

    void updatePhysics(float deltaTime, const std::vector<Block> &blocks);

    std::vector<Block> getNextBlockLookingAt(const std::vector<Block> &blocks, std::map<std::string, unsigned int>);

    ~Camera();
};
