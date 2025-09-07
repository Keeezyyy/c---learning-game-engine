
#include "camera.h"

#include "utils.h"


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
            keys[key] = true;
        else if (action == GLFW_RELEASE)
            keys[key] = false;
    }
}

void Camera::processInput(float deltaTime)
{
    float moveSpeed = 5.0f;

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
    cameraPos.y += direction.y * moveSpeed * deltaTime;
    cameraPos.z += direction.z * moveSpeed * deltaTime;
}

Camera::~Camera() {}
