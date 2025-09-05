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

int screenwidth = 640;
int screenheight = 480;

enum class BlockType
{
    AIR,
    GRASS,
    DIRT,
    STONE
};

// 2. Blockfaces
enum class BlockFace
{
    Top,
    Bottom,
    Left,
    Right,
    Front,
    Back
};

// 3. Struktur für einen Vertex
struct Vertex
{
    float x, y, z; // Position
    float u, v;    // Texturkoordinaten
};

// 4. BlockFaceData speichert 6 Vertices pro Seite
struct BlockFaceData
{
    Vertex vertices[6];  // 2 Dreiecke = 6 Vertices
    std::string texture; // Textur der Seite
    unsigned int textureID;
};

struct vector3Int
{
    int x, y, z;
};

// 5. Block speichert Typ und die 6 Seiten
struct Block
{
    BlockType type;
    BlockFaceData faces[6]; // Top, Bottom, Left, Right, Front, Back
    vector3Int wordPos;

    // Initialisierung der Texturen pro Face
    void init(BlockType t, const std::array<std::string, 6> &textures, const std::array<unsigned int, 6> &textureIds, int x, int y, int z)
    {
        wordPos.x = x;
        wordPos.y = y;
        wordPos.z = z;
        type = t;
        for (int i = 0; i < 6; i++)
        {
            faces[i].texture = textures[i];
            faces[i].textureID = textureIds[i];

            switch (i)
            {
            case (int)BlockFace::Top:
                faces[i].vertices[0] = {0, 1, 0, 0, 0};
                faces[i].vertices[1] = {1, 1, 0, 1, 0};
                faces[i].vertices[2] = {1, 1, 1, 1, 1};
                faces[i].vertices[3] = {0, 1, 0, 0, 0};
                faces[i].vertices[4] = {1, 1, 1, 1, 1};
                faces[i].vertices[5] = {0, 1, 1, 0, 1};
                break;

            case (int)BlockFace::Bottom:
                faces[i].vertices[0] = {0, 0, 0, 0, 0};
                faces[i].vertices[1] = {1, 0, 1, 1, 1};
                faces[i].vertices[2] = {1, 0, 0, 1, 0};
                faces[i].vertices[3] = {0, 0, 0, 0, 0};
                faces[i].vertices[4] = {0, 0, 1, 0, 1};
                faces[i].vertices[5] = {1, 0, 1, 1, 1};
                break;

            case (int)BlockFace::Left:
                faces[i].vertices[0] = {0, 0, 0, 0, 0};
                faces[i].vertices[1] = {0, 1, 1, 1, 1};
                faces[i].vertices[2] = {0, 1, 0, 1, 0};
                faces[i].vertices[3] = {0, 0, 0, 0, 0};
                faces[i].vertices[4] = {0, 0, 1, 0, 1};
                faces[i].vertices[5] = {0, 1, 1, 1, 1};
                break;

            case (int)BlockFace::Right:
                faces[i].vertices[0] = {1, 0, 0, 0, 0};
                faces[i].vertices[1] = {1, 1, 0, 1, 0};
                faces[i].vertices[2] = {1, 1, 1, 1, 1};
                faces[i].vertices[3] = {1, 0, 0, 0, 0};
                faces[i].vertices[4] = {1, 1, 1, 1, 1};
                faces[i].vertices[5] = {1, 0, 1, 0, 1};
                break;

            case (int)BlockFace::Front:
                faces[i].vertices[0] = {0, 0, 1, 0, 0};
                faces[i].vertices[1] = {1, 1, 1, 1, 1};
                faces[i].vertices[2] = {1, 0, 1, 1, 0};
                faces[i].vertices[3] = {0, 0, 1, 0, 0};
                faces[i].vertices[4] = {0, 1, 1, 0, 1};
                faces[i].vertices[5] = {1, 1, 1, 1, 1};
                break;

            case (int)BlockFace::Back:
                faces[i].vertices[0] = {0, 0, 0, 0, 0};
                faces[i].vertices[1] = {1, 0, 0, 1, 0};
                faces[i].vertices[2] = {1, 1, 0, 1, 1};
                faces[i].vertices[3] = {0, 0, 0, 0, 0};
                faces[i].vertices[4] = {1, 1, 0, 1, 1};
                faces[i].vertices[5] = {0, 1, 0, 0, 1};
                break;
            }
        }
    }
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

u_char *readPng(const char *filename, int *out_width, int *out_height, int *out_colorCHannels)
{
    unsigned char *bytes = stbi_load(filename, out_width, out_height, out_colorCHannels, 0);

    return bytes;
}

void getTexturesArray(std::map<std::string, unsigned int> &map,
                      std::array<unsigned int, 6> *array_out,
                      std::array<std::string, 6> &array_in)
{
    for (int i = 0; i < 6; i++) // Schleife bis 6, nicht 7
    {
        (*array_out)[i] = map[array_in[i]];
    }

    // Array ausgeben
    std::cout << "Array: ";
    for (int i = 0; i < 6; i++)
    {
        std::cout << (*array_out)[i] << " ";
    }
    std::cout << std::endl;
}

void fillVerticesWithBlocks(
    float arr_out[],
    Block *blocks,
    int numBlocks)
{
    int index = 0; // Zeiger für arr_out

    for (int i = 0; i < numBlocks; i++)
    {
        Block b = blocks[i];

        for (int k = 0; k < 6; k++)
        {
            BlockFaceData face = b.faces[k];

            for (int j = 0; j < 6; j++)
            {
                Vertex v = face.vertices[j];

                // Vertex
                arr_out[index++] = v.x;
                arr_out[index++] = v.y;
                arr_out[index++] = v.z;
                arr_out[index++] = v.u;
                arr_out[index++] = v.v;

                // TextureId
                arr_out[index++] = face.textureID;

                // World Pos
                arr_out[index++] = (float)b.wordPos.x;
                arr_out[index++] = (float)b.wordPos.y;
                arr_out[index++] = (float)b.wordPos.z;
            }
        }
    }
}

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

void loadTextures(std::map<std::string, unsigned int> &map)
{
    std::cout << "loading textures ... \n";
    std::string path = "./textures";

    DIR *dir = opendir(path.c_str());
    if (!dir)
    {
        std::cerr << "Ordner nicht gefunden!\n";
        return;
    }

    int idx = 0; // Texture Unit Zähler
    struct dirent *entry;
    while ((entry = readdir(dir)) != nullptr)
    {
        if (entry->d_type == DT_REG)
        {
            std::string fileName = entry->d_name;
            std::string fullPath = path + "/" + fileName;

            unsigned int texID;
            glGenTextures(1, &texID);
            glActiveTexture(GL_TEXTURE0 + idx);  // Texture Unit aktivieren
            glBindTexture(GL_TEXTURE_2D, texID); // Textur binden

            // Wrap / Filter Einstellungen
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            int width, height, nrChannels;
            unsigned char *data = readPng(fullPath.c_str(), &width, &height, &nrChannels);

            if (data)
            {
                GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
                glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
                glGenerateMipmap(GL_TEXTURE_2D);

                map[fileName] = texID;
            }
            else
            {
                std::cerr << "Failed to load texture: " << fileName << "\n";
            }

            stbi_image_free(data);
            idx++;
        }
    }
    closedir(dir);
    std::cout << "[finished] loading textures \n";
}

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

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    const float speed = 0.5f; // Geschwindigkeit der Bewegung

    if (action == GLFW_PRESS || action == GLFW_REPEAT) // Reagiere auch auf gehaltene Taste
    {
        switch (key)
        {
        case GLFW_KEY_W:
            playerCam.cameraPos.z -= speed; // Vorwärts
            break;
        case GLFW_KEY_S:
            playerCam.cameraPos.z += speed; // Rückwärts
            break;
        case GLFW_KEY_A:
            playerCam.cameraPos.x -= speed; // Nach links
            break;
        case GLFW_KEY_D:
            playerCam.cameraPos.x += speed; // Nach rechts
            break;
        case GLFW_KEY_SPACE:
            playerCam.cameraPos.y += speed; // Hochspringen
            break;
        case GLFW_KEY_LEFT_SHIFT:
            playerCam.cameraPos.y -= speed; // Hochspringen
            break;
        default:
            break;
        }
        printf("Neue Position: x=%.2f, y=%.2f, z=%.2f\n",
               playerCam.cameraPos.x, playerCam.cameraPos.y, playerCam.cameraPos.z);
    }
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
    char *vertexShaderSource = readFile("shader/vertexShader.vert"), *FragmentShaderSource = readFile("shader/vertexShader.frag");

    if (!vertexShaderSource || !FragmentShaderSource)
    {
        std::cout << "Shader files not found!" << std::endl;
        return -1;
    }
    // start width Buffer connections

    // Texture Map
    std::map<std::string, unsigned int> texture_map;
    // Textures

    // loading textures
    loadTextures(texture_map);

    // Game initialisation

    Block dirtBlock;

    std::array<std::string, 6> grassTextures = {
        "grass_top.png", "grass_top.png", "grass_top.png",
        "grass_top.png", "grass_side.png", "grass_side.png"};

    std::array<unsigned int, 6> grassTextureIds;

    getTexturesArray(texture_map, &grassTextureIds, grassTextures);

    dirtBlock.init(BlockType::DIRT, grassTextures, grassTextureIds, 0, -2, -1);

    float vertecies[6 * 6 * 9 * 1]; // 6 Faces, 6 Vertices pro Face, 9 floats pro Vertex

    Block blocks[1] = {dirtBlock};
    fillVerticesWithBlocks(vertecies, blocks, 1);

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
    glShaderSource(fragShader, 1, &FragmentShaderSource, NULL);
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

    free(vertexShaderSource);
    free(FragmentShaderSource);
    while (!glfwWindowShouldClose(window))
    {
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
        for (int i = 0; i < 16; i++) samplers[i] = i; // [0,1,2,...,15]
        glUniform1iv(glGetUniformLocation(shaderProgram, "ourTexture"), 16, samplers);

        // Matrizen an den Shader senden
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDetachShader(shaderProgram, vertextShader);
    glDetachShader(shaderProgram, fragShader);

    glfwDestroyWindow(window);
    glfwTerminate();
}