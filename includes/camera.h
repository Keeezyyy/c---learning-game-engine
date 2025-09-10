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
#include <optional>
#include <chrono>


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

    glm::vec3 looking_at_coord; // Forward
    BlockFace looking_at_face;

    std::vector<Block> blocks;

    float sizeDown = 1.7f;

    float JumpVelocity = 0.0f;
    bool isJumping = false;

    std::chrono::time_point<std::chrono::high_resolution_clock> lastTimePlacedBlock;

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

    float getGroundHeight();

    void updatePhysics(float deltaTime);

    void getNextBlockLookingAt();

    const char *faceToString(BlockFace);

    void interact_with_block(glm::vec3 *outVec, int *outInt);

    void checkForBlockFacing();

    glm::vec3 resolveCollision(glm::vec3 desiredPos, glm::vec3 oldPos);

    ~Camera();
};
