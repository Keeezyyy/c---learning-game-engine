#include <optional>

#include "camera.h"

#include "utils.h"
#include "block.h"
#include "world.h"

Camera::Camera(int width, int height)
{

    screenwidth = width;
    screenheight = height;

    float lastX = screenwidth / 2.0f;
    float lastY = screenheight / 2.0f;
}

// Statischer Callback
void Camera::mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
    Camera *cam = static_cast<Camera *>(glfwGetWindowUserPointer(window));
    if (cam)
        cam->handleMouse(xpos, ypos);
}

// Member-Funktion, die die Maus verarbeitet
void Camera::handleMouse(double xpos, double ypos)
{
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

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

    V = {direction.x, direction.y, direction.z};

    glm::vec3 worldUp = {0.0f, 1.0f, 0.0f};
    glm::vec3 right = glm::normalize(glm::cross(direction, worldUp));
    glm::vec3 up = glm::normalize(glm::cross(right, direction));

    U = {right.x, right.y, right.z};
    N = {up.x, up.y, up.z};
}

glm::vec3 Camera::place_block()
{
    glm::vec3 out;

    out.x = 0.989;
    out.y = 0.989;
    out.z = 0.989;

    auto now = std::chrono::high_resolution_clock::now();
    if (keys[GLFW_KEY_1] && abs(std::chrono::duration_cast<std::chrono::milliseconds>(now - lastTimePlacedBlock).count()) > 200)
    {

        switch (looking_at_face)
        {
        case BlockFace::Top:
            out.x = looking_at_coord.x;
            out.y = looking_at_coord.y + 1;
            out.z = looking_at_coord.z;
            break;

        case BlockFace::Bottom:
            out.x = looking_at_coord.x;
            out.y = looking_at_coord.y - 1;
            out.z = looking_at_coord.z;
            break;

        case BlockFace::Left:
            out.x = looking_at_coord.x - 1;
            out.y = looking_at_coord.y;
            out.z = looking_at_coord.z;
            break;

        case BlockFace::Right:
            out.x = looking_at_coord.x + 1;
            out.y = looking_at_coord.y;
            out.z = looking_at_coord.z;
            break;

        case BlockFace::Front:
            out.x = looking_at_coord.x;
            out.y = looking_at_coord.y;
            out.z = looking_at_coord.z + 1;
            break;

        case BlockFace::Back:
            out.x = looking_at_coord.x;
            out.y = looking_at_coord.y;
            out.z = looking_at_coord.z - 1;
            break;

        default:
            break;
        }
    }

    return out;
}

const char *Camera::faceToString(BlockFace f)
{
    switch (f)
    {
    case BlockFace::Left:
        return "Left";
    case BlockFace::Right:
        return "Right";
    case BlockFace::Top:
        return "Top";
    case BlockFace::Bottom:
        return "Bottom";
    case BlockFace::Front:
        return "Front";
    case BlockFace::Back:
        return "Back";
    default:
        return "Unknown";
    }
}

// Verbesserter Raycasting-Algorithmus (3D DDA) für Blockauswahl und Face-Erkennung
void Camera::getNextBlockLookingAt()
{
    // Maximale Reichweite des Raycasts
    const float maxDistance = 10.0f;
    const float epsilon = 1e-4f;

    glm::vec3 rayOrigin = cameraPos;
    glm::vec3 rayDir = glm::normalize(V);

    // Startposition im Blockgitter (ganzzahlig)
    glm::ivec3 blockPos = glm::floor(rayOrigin);

    // Schrittweite in jede Richtung
    glm::ivec3 step;
    step.x = (rayDir.x > 0) ? 1 : (rayDir.x < 0 ? -1 : 0);
    step.y = (rayDir.y > 0) ? 1 : (rayDir.y < 0 ? -1 : 0);
    step.z = (rayDir.z > 0) ? 1 : (rayDir.z < 0 ? -1 : 0);

    // tMax: Abstand bis zur nächsten Blockkante in jeder Achse
    glm::vec3 tMax;
    glm::vec3 tDelta;

    for (int i = 0; i < 3; ++i) {
        if (rayDir[i] != 0.0f) {
            float nextBoundary = (step[i] > 0) ? (blockPos[i] + 1.0f) : (blockPos[i]);
            tMax[i] = (nextBoundary - rayOrigin[i]) / rayDir[i];
            tDelta[i] = 1.0f / std::abs(rayDir[i]);
        } else {
            tMax[i] = std::numeric_limits<float>::infinity();
            tDelta[i] = std::numeric_limits<float>::infinity();
        }
    }

    float distanceTravelled = 0.0f;
    BlockFace lastFace = BlockFace::Front; // Default, wird überschrieben

    // Hilfsfunktion: Prüft, ob ein Block an blockPos existiert
    auto blockExists = [&](const glm::ivec3& pos) -> const Block* {
        for (const Block& b : blocks) {
            glm::ivec3 bpos = glm::ivec3(b.wordPos.x, b.wordPos.y, b.wordPos.z);
            if (bpos == pos)
                return &b;
        }
        return nullptr;
    };

    while (distanceTravelled < maxDistance) {
        // Block an aktueller Position?
        const Block* hitBlock = blockExists(blockPos);
        if (hitBlock) {
            // Treffer! Face bestimmen anhand der zuletzt überschrittenen Achse
            looking_at_coord = glm::vec3(blockPos);
            looking_at_face = lastFace;

            printf("Looking at block (%d, %d, %d), face: %s\n",
                   blockPos.x, blockPos.y, blockPos.z, faceToString(lastFace));
            return;
        }

        // Nächste Achse bestimmen, die überschritten wird
        if (tMax.x < tMax.y && tMax.x < tMax.z) {
            blockPos.x += step.x;
            distanceTravelled = tMax.x;
            tMax.x += tDelta.x;
            lastFace = (step.x > 0) ? BlockFace::Left : BlockFace::Right;
        } else if (tMax.y < tMax.z) {
            blockPos.y += step.y;
            distanceTravelled = tMax.y;
            tMax.y += tDelta.y;
            lastFace = (step.y > 0) ? BlockFace::Bottom : BlockFace::Top;
        } else {
            blockPos.z += step.z;
            distanceTravelled = tMax.z;
            tMax.z += tDelta.z;
            lastFace = (step.z > 0) ? BlockFace::Back : BlockFace::Front;
        }
    }

    // Kein Block getroffen
    looking_at_face = BlockFace::Front;
    looking_at_coord = glm::vec3(0.989f, 0.989f, 0.989f);
}

// Statischer Callback
void Camera::key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    Camera *cam = static_cast<Camera *>(glfwGetWindowUserPointer(window));
    if (cam)
        cam->handleKey(key, scancode, action, mods);
}

// Instanz-Funktion
void Camera::handleKey(int key, int scancode, int action, int mods)
{
    if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS)
        {
            keys[key] = true;

            if (key == GLFW_KEY_SPACE) // hier muss key verglichen werden, nicht action
            {
                isJumping = true;
            }
        }
        else if (action == GLFW_RELEASE)
        {
            keys[key] = false;

            if (key == GLFW_KEY_SPACE)
            {
                isJumping = false; // optional: Reset beim Loslassen
            }
        }
    }
}

void Camera::processInput(float deltaTime)
{

    float moveSpeed = 5.0f;
    if (JumpVelocity && abs(JumpVelocity) < 1.2)
    {
        if (abs(JumpVelocity) > 1)
        {

            moveSpeed /= abs(JumpVelocity);
        }
    }

    if (abs(JumpVelocity) > 1.2)
    {
        moveSpeed /= 1.2;
    }

    glm::vec3 direction(0.0f);

    if (keys[GLFW_KEY_W])
        direction += glm::vec3(V.x, V.y, V.z);
    if (keys[GLFW_KEY_S])
        direction -= glm::vec3(V.x, V.y, V.z);
    if (keys[GLFW_KEY_A])
        direction -= glm::vec3(U.x, U.y, U.z);
    if (keys[GLFW_KEY_D])
        direction += glm::vec3(U.x, U.y, U.z);

    if (glm::length(direction) > 0.0f)
        direction = glm::normalize(direction);

    cameraPos.x += direction.x * moveSpeed * deltaTime;

    cameraPos.z += direction.z * moveSpeed * deltaTime;
}

void Camera::updatePhysics(float deltaTime)
{
    const float gravity = -9.8f;
    const float jumpStrength = 5.0f;
    const float playerHeight = sizeDown; // Höhe von Fuß -> Kamera (anpassen)
    const float eps = 0.001f;

    float groundY = getGroundHeight(); // höchster Block unter Spieler (Bodenhöhe)

    float footY = cameraPos.y - playerHeight; // y der "Füße"
    bool onGround = footY <= groundY + eps;

    // Sprung starten (kein nudge, nur Velocity setzen)
    if (keys[GLFW_KEY_SPACE] && !isJumping && onGround)
    {
        JumpVelocity = jumpStrength;
        isJumping = true;
    }

    // Gravitation / Integration: berechne geplante neue Position
    if (!onGround || isJumping)
    {
        JumpVelocity += gravity * deltaTime;
        float nextY = cameraPos.y + JumpVelocity * deltaTime;

        // Wenn die nächste Position in/unter den Boden gehen würde --> clampen
        if (nextY - playerHeight <= groundY + eps)
        {
            // Landung
            cameraPos.y = groundY + playerHeight;
            JumpVelocity = 0.0f;
            isJumping = false;
            // optional: onGround = true;
        }
        else
        {
            cameraPos.y = nextY;
        }
    }
    else
    {
        // Auf dem Boden stehen, stelle sicher, dass Kamera exakt auf Bodenhöhe ist
        cameraPos.y = groundY + playerHeight;
        JumpVelocity = 0.0f;
        isJumping = false;
    }
}

float Camera::getGroundHeight()
{
    float groundY = 0.0f; // Standard-Bodenhöhe

    float feetY = cameraPos.y - sizeDown;

    for (const Block &b : blocks)
    {
        float blockTop = static_cast<float>(b.wordPos.y) + 1.0f;
        int x = (int)b.wordPos.x;
        int z = (int)b.wordPos.z;

        if ((int)cameraPos.x == x && (int)cameraPos.z == z)
        {

            if (groundY < blockTop)
            {

                groundY = blockTop;
            }
        }
    }

    return groundY;
}

Camera::~Camera() {}
