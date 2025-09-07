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
#include "buffer.h"

Buffer::Buffer()
{
    VAO = 0;
    VBO = 0;
}

void Buffer::VBOgen(int size, float *vertecies)
{
    // Lösche alte VAO und VBO falls sie existieren
    if (VAO != 0)
    {
        glDeleteVertexArrays(1, &VAO);
        VAO = 0;
    }
    if (VBO != 0)
    {
        glDeleteBuffers(1, &VBO);
        VBO = 0;
    }

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, size, vertecies, GL_DYNAMIC_DRAW);
}

void Buffer::update(int size, float *vertecies)
{
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, size, vertecies);
}

void Buffer::VertexInterpretation()
{
    // Vertext interpretation
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void *)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(3);
}

Buffer::~Buffer() {}