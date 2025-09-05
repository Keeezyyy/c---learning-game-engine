#include <GLFW/glfw3.h>
#include <cmath>
#include <iostream>
#include <thread>
#include <chrono>

struct vec3
{
    float x, y, z;
};

struct triangle
{
    vec3 points[3];
};

struct mat4x4
{
    float m[4][4] = {{0}};
};
struct mat3x3
{
    float m[3][3] = {{0}};
};
// Erster sehr kleiner Würfel (Skalierung 0.2)
triangle projCube[] = {
    // Front (+Z)
    {{{0.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f}}},
    {{{0.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 1.0f}}},

    // Back (-Z)
    {{{0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 0.0f}}},
    {{{0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}}},

    // Left (-X)
    {{{0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 1.0f}}},
    {{{0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}}},

    // Right (+X)
    {{{1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}}},
    {{{1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 1.0f}}},

    // Top (+Y)
    {{{0.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}}},
    {{{0.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 1.0f}}},

    // Bottom (-Y)
    {{{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 1.0f}}},
    {{{0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}}},
};

// Zweiter sehr kleiner Würfel (Skalierung 0.2, verschoben um 0.3 nach X)

int screenWidth = 640;
int screenHeight = 480;

float aspectRatio = (float)screenHeight / (float)screenWidth;
float fov = 90.0f;
float fFar = 1000.0f;
float fNear = 0.5f;

float fovRad = 1.0f / tanf(fov * 0.5f / 180.0f * 3.14159f);

void fillProjMatrix(mat4x4 *matrix)
{
    matrix->m[0][0] = aspectRatio * fovRad;
    matrix->m[1][1] = fovRad;

    matrix->m[2][2] = (fFar) / (fFar - fNear);
    matrix->m[2][3] = 1.0f;

    matrix->m[3][2] = (-fFar * fNear) / (fFar - fNear);

    matrix->m[3][3] = 0.0f;
}

void fillRotationMatrixes(mat3x3 *xRot, mat3x3 *yRot, mat3x3 *zRot, vec3 rotation)
{
    xRot->m[0][0] = 1;
    xRot->m[1][1] = cos(rotation.x);
    xRot->m[1][2] = -1 * sin(rotation.x);
    xRot->m[2][1] = sin(rotation.x);
    xRot->m[2][2] = cos(rotation.x);

    yRot->m[0][0] = cos(rotation.y);
    yRot->m[0][2] = sin(rotation.y);
    yRot->m[1][1] = 1;
    yRot->m[2][0] = -1 * sin(rotation.y);
    yRot->m[2][2] = cos(rotation.y);

    zRot->m[0][0] = cos(rotation.z);
    zRot->m[0][1] = -1 * sin(rotation.z);
    zRot->m[1][0] = sin(rotation.z);
    zRot->m[1][1] = cos(rotation.z);
    zRot->m[2][2] = 1;
}

void fillCamera(vec3 &u, vec3 &n, vec3 &v, vec3 &pos, mat4x4 *matrix)
{
    matrix->m[0][0] = u.x;
    matrix->m[0][1] = u.y;
    matrix->m[0][2] = u.z;
    matrix->m[0][3] = -pos.x;

    matrix->m[1][0] = v.x;
    matrix->m[1][1] = v.y;
    matrix->m[1][2] = v.z;
    matrix->m[1][3] = -pos.y;

    matrix->m[2][0] = n.x;
    matrix->m[2][1] = n.y;
    matrix->m[2][2] = n.z;
    matrix->m[2][3] = -pos.z;

    matrix->m[3][0] = 0.0f;
    matrix->m[3][1] = 0.0f;
    matrix->m[3][2] = 0.0f;
    matrix->m[3][3] = 1.0f;
}

void multiplyByMatrix(vec3 &Vector, vec3 *out, mat4x4 &matrix)
{
    // std::cout << "===== Starte Matrix * Vektor Berechnung =====\n";
    // std::cout << "Input Vektor: (" << Vector.x << ", " << Vector.y << ", " << Vector.z << ")\n";

    float values[4] = {Vector.x, Vector.y, Vector.z, 1.0f}; // homogener Vektor

    float result[4] = {0.0f, 0.0f, 0.0f, 0.0f};

    for (int i = 0; i < 4; i++)
    {
        float sum = 0.0f;

        for (int k = 0; k < 4; k++)
        {
            float val = values[k] * matrix.m[i][k];
            sum += val;

            // std::cout << "  result[" << i << "] += values[" << k << "] * matrix[" << i << "][" << k << "]\n";
            // std::cout << "              = " << values[k] << " * " << matrix.m[i][k] << " = " << val << "\n";
        }

        result[i] = sum;
        // std::cout << "=> result[" << i << "] = " << sum << "\n\n";
    }

    vec3 final;
    if (result[3] != 0.0f)
    {
        final.x = result[0] / result[3];
        final.y = result[1] / result[3];
        final.z = result[2] / result[3];
        // std::cout << "Normiere durch w = " << result[3] << "\n";
    }
    else
    {
        final.x = result[0];
        final.y = result[1];
        final.z = result[2];
        // std::cout << "Kein w gefunden, benutze rohe Werte\n";
    }

    // std::cout << "Output Vektor: (" << final.x << ", " << final.y << ", " << final.z << ")\n";
    // std::cout << "===== Berechnung abgeschlossen =====\n";

    out->x = final.x;
    out->y = final.y;
    out->z = final.z;
}

void multiplyVecByVerticy(const vec3 &i, vec3 *o, const mat3x3 &matrix)
{
    // temporäre Kopie der Eingabewerte verhindert in-place-Side-Effects
    float x = i.x;
    float y = i.y;
    float z = i.z;

    o->x = x * matrix.m[0][0] + y * matrix.m[0][1] + z * matrix.m[0][2];
    o->y = x * matrix.m[1][0] + y * matrix.m[1][1] + z * matrix.m[1][2];
    o->z = x * matrix.m[2][0] + y * matrix.m[2][1] + z * matrix.m[2][2];
}

mat4x4 projectionMatrix;

mat3x3 xRotationMatrix;
mat3x3 yRotationMatrix;
mat3x3 zRotationMatrix;

vec3 cameraPos = {0.0f, 5.0f, 10.0f};
vec3 U = {1.0f, 0.0f, 0.0f};
vec3 N = {0.0f, 1.0f, 0.0f};
vec3 V = {0.0f, 0.0f, 1.0f};

mat4x4 cameraMatrix;

int main()
{
    fillProjMatrix(&projectionMatrix);

    fillCamera(U, N, V, cameraPos, &cameraMatrix);

    if (!glfwInit())
        return -1;

    GLFWwindow *window = glfwCreateWindow(640, 480, "Triangle", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    while (!glfwWindowShouldClose(window))
    {

        float speed = 0.05f;
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            cameraPos.z -= speed;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            cameraPos.z += speed;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            cameraPos.x -= speed;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            cameraPos.x += speed;
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
            cameraPos.y += speed;
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            cameraPos.y -= speed;

        // Kamera-Matrix aktualisieren
        fillCamera(U, N, V, cameraPos, &cameraMatrix);
        glClear(GL_COLOR_BUFFER_BIT);

        // Setze die Projektion: Normalisierte Koordinaten von -1 bis 1
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, 640, 0, 480, -1, 1); // (0,0) unten links, (640,480) oben rechts
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glColor3f(1.0f, 0.0f, 0.0f); // Rot

        float time = (float)glfwGetTime();
        vec3 rot = {time * 0.0f, time * 0.0f, time * 0.0f};

        for (auto tri : projCube)
        {
            triangle triProjected, triRotated;

            mat3x3 xRot, yRot, zRot;

            fillRotationMatrixes(&xRot, &yRot, &zRot, rot);

            multiplyVecByVerticy(tri.points[0], &triRotated.points[0], xRot);
            multiplyVecByVerticy(tri.points[1], &triRotated.points[1], xRot);
            multiplyVecByVerticy(tri.points[2], &triRotated.points[2], xRot);

            multiplyVecByVerticy(triRotated.points[0], &triRotated.points[0], yRot);
            multiplyVecByVerticy(triRotated.points[1], &triRotated.points[1], yRot);
            multiplyVecByVerticy(triRotated.points[2], &triRotated.points[2], yRot);

            multiplyVecByVerticy(triRotated.points[0], &triRotated.points[0], zRot);
            multiplyVecByVerticy(triRotated.points[1], &triRotated.points[1], zRot);
            multiplyVecByVerticy(triRotated.points[2], &triRotated.points[2], zRot);

            triRotated.points[0].z += 3.0f;
            triRotated.points[1].z += 3.0f;
            triRotated.points[2].z += 3.0f;

            triangle tmp;

            multiplyByMatrix(triRotated.points[0], &tmp.points[0], cameraMatrix);
            multiplyByMatrix(triRotated.points[1], &tmp.points[1], cameraMatrix);
            multiplyByMatrix(triRotated.points[2], &tmp.points[2], cameraMatrix);

            vec3 line1, line2, normal;

            line1.x = triRotated.points[1].x - triRotated.points[0].x;
            line1.y = triRotated.points[1].y - triRotated.points[0].y;
            line1.z = triRotated.points[1].z - triRotated.points[0].z;

            line2.x = triRotated.points[2].x - triRotated.points[0].x;
            line2.y = triRotated.points[2].y - triRotated.points[0].y;
            line2.z = triRotated.points[2].z - triRotated.points[0].z;

            normal.x = line1.y * line2.z - line1.z * line2.y;
            normal.y = line1.z * line2.x - line1.x * line2.z;
            normal.z = line1.x * line2.y - line1.y * line2.x;

            float l = sqrtf(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);

            normal.x /= l;
            normal.y /= l;
            normal.z /= l;

            if (true)
            {
                multiplyByMatrix(tmp.points[0], &triProjected.points[0], projectionMatrix);
                multiplyByMatrix(tmp.points[1], &triProjected.points[1], projectionMatrix);
                multiplyByMatrix(tmp.points[2], &triProjected.points[2], projectionMatrix);

                triProjected.points[0].x += 1.0f;
                triProjected.points[0].y += 1.0f;
                triProjected.points[1].x += 1.0f;
                triProjected.points[1].y += 1.0f;
                triProjected.points[2].x += 1.0f;
                triProjected.points[2].y += 1.0f;

                triProjected.points[0].x *= 0.5 * (float)screenWidth;
                triProjected.points[0].y *= 0.5 * (float)screenHeight;
                triProjected.points[1].x *= 0.5 * (float)screenWidth;
                triProjected.points[1].y *= 0.5 * (float)screenHeight;
                triProjected.points[2].x *= 0.5 * (float)screenWidth;
                triProjected.points[2].y *= 0.5 * (float)screenHeight;

                for (int i = 0; i < 3; i++)
                {
                    std::cout << "Point " << i
                              << ": x = " << triProjected.points[i].x
                              << ", y = " << triProjected.points[i].y
                              << std::endl;
                }

                glBegin(GL_LINE_LOOP);
                glVertex2f(triProjected.points[0].x, triProjected.points[0].y);
                glVertex2f(triProjected.points[1].x, triProjected.points[1].y);
                glVertex2f(triProjected.points[2].x, triProjected.points[2].y);
                glEnd();
            }
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
