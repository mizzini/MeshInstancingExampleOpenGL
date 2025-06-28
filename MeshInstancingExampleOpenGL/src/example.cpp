// Instancing example with multiple meshes.
// Pressing space key while running switches between cube and pyramid meshes.
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <iostream>
#include <fstream>
#include "utils.h"
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/type_ptr.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>

#define numVAOs 1
#define numVBOs 2

GLuint renderingProgram;
GLuint vao[numVAOs];
GLuint vbo[numVBOs];

float cameraX, cameraY, cameraZ;
float cubeLocX, cubeLocY, cubeLocZ;

// Display vars
GLuint mvLoc, pLoc, vLoc, tfLoc;
int width, height, i;
float aspect, tf;

glm::mat4 pMat, vMat, mMat, tMat, rMat, mvMat;

int currentShape = 0; // 0 for cube and 1 for pyramid.
bool pressedLastFrame = false;

void setupVertices(void)
{
    float vertexPositions[108] = {
        // Front face
        -1.0f, -1.0f,  1.0f,   // lower-left
         1.0f, -1.0f,  1.0f,   // lower-right
         1.0f,  1.0f,  1.0f,   // upper-right

         1.0f,  1.0f,  1.0f,   // upper-right
        -1.0f,  1.0f,  1.0f,   // upper-left
        -1.0f, -1.0f,  1.0f,   // lower-left

        // Back face
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,

         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,

        // Left face
        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,

        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,

        // Right face
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,

         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,

         // Top face
         -1.0f,  1.0f, -1.0f,
         -1.0f,  1.0f,  1.0f,
          1.0f,  1.0f,  1.0f,

          1.0f,  1.0f,  1.0f,
          1.0f,  1.0f, -1.0f,
         -1.0f,  1.0f, -1.0f,

         // Bottom face
         -1.0f, -1.0f, -1.0f,
          1.0f, -1.0f, -1.0f,
          1.0f, -1.0f,  1.0f,

          1.0f, -1.0f,  1.0f,
         -1.0f, -1.0f,  1.0f,
         -1.0f, -1.0f, -1.0f
    };

    float pyramidVertexPositions[54] = {
        // Front face
        0.0f, 1.0f, 0.0f,
        -1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, 1.0f,

        // Right face
        0.0f, 1.0f, 0.0f,
        1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, -1.0f,

        // Back face
        0.0f, 1.0f, 0.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,

        // Left face
        0.0f, 1.0f, 0.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, 1.0f,

        // Base triangle 1
        -1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, -1.0f,

        // Base triangle 2
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, 1.0f
    };

    glGenVertexArrays(1, vao);
    glBindVertexArray(vao[0]);
    glGenBuffers(numVBOs, vbo);

    // Cubes
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPositions), vertexPositions, GL_STATIC_DRAW);

    // Pyramids
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidVertexPositions), pyramidVertexPositions, GL_STATIC_DRAW);
}

void init(GLFWwindow* window)
{
    renderingProgram = createShaderProgram("./glsl/vertShader.glsl", "./glsl/fragShader.glsl");

    cameraX = 0.0f;
    cameraY = 0.0f;
    cameraZ = 420.0f;

    cubeLocX = 0.0f;
    cubeLocY = -2.0f;
    cubeLocZ = 0.0f;

    // Set up vertices for cube.
    setupVertices();

    // Adjust OpenGL settings and draw model.
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
}

void display(GLFWwindow* window, double currentTime)
{
    glClear(GL_DEPTH_BUFFER_BIT);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(renderingProgram);

    // Get uniform variables for the MV and projection matrices.
    // These will be defined in GLSL.
    mvLoc = glGetUniformLocation(renderingProgram, "mv_matrix");
    pLoc = glGetUniformLocation(renderingProgram, "p_matrix");
    vLoc = glGetUniformLocation(renderingProgram, "v_matrix");

    // Build perspective matrix.
    glfwGetFramebufferSize(window, &width, &height);
    aspect = (float)width / (float)height;
    pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f); // 1.04... is 60 deg in radians.

    // Build view matrix, model matrix, and model-view matrix.
    vMat = glm::translate(glm::mat4(1.0f), glm::vec3(-cameraX, -cameraY, -cameraZ)); // Represents the camera's position.

    // Copy perspective and MV matrices to uniform variables.
    glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
    glUniformMatrix4fv(pLoc, 1, GL_FALSE, glm::value_ptr(pMat));
    glUniformMatrix4fv(vLoc, 1, GL_FALSE, glm::value_ptr(vMat));

    tf = (float)currentTime;
    tfLoc = glGetUniformLocation(renderingProgram, "tf");
    glUniform1f(tfLoc, (float)tf);

    // Track current mesh to switch between.
    if (currentShape == 0) {
        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(0);
        glDrawArraysInstanced(GL_TRIANGLES, 0, 36, 100000);
    }
    else {
        glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(0);
        glDrawArraysInstanced(GL_TRIANGLES, 0, 18, 100000);
    }
}

int main()
{
    if (!glfwInit()) { exit(EXIT_FAILURE); }

    // Set version that machine must be compatible with
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    // Create the window
    GLFWwindow* window = glfwCreateWindow(600, 600, "Example", NULL, NULL);
    glfwMakeContextCurrent(window);

    // Check that GLAD is loaded
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "GLAD failed to initialize!" << std::endl;
        return EXIT_FAILURE;
    }
    std::cout << "GLAD initialized successfully!" << std::endl;

    // Enables v-sync.
    // Must be called after the OpenGL context, but before the render loop.
    glfwSwapInterval(1);

    // Call previously defined init function for app-specific initialization.
    init(window);

    while (!glfwWindowShouldClose(window))
    {
        int state = glfwGetKey(window, GLFW_KEY_SPACE);
        if (state == GLFW_PRESS && !pressedLastFrame)
        {
            currentShape = 1 - currentShape;
        }
        pressedLastFrame = (state == GLFW_PRESS);

        // Render
        display(window, glfwGetTime());
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    std::cout << "All done." << std::endl;

    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
