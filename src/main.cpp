#include <bits/stdc++.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "shader.h"
#include "camera.h"

using namespace std;

// Screen Dimensions
const unsigned int SCR_WIDTH = 800, SCR_HEIGHT = 600;

float prism_angle = 0.0f, delta = 0.0f, last = 0.0f;
const float prism_rotate_speed = 2.5f;

// Camera preset positions
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
glm::vec3 preset1 = glm::vec3(5.0f, 5.0f, 5.0f);
glm::vec3 preset2 = glm::vec3(0.0f, 0.0f, 3.0f);

bool rotate_prism = false;

// Center of Polygon obj
glm::vec3 center = glm::vec3(0.0f, 0.0f, 0.0f);

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// Rotate Prism
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS && key == GLFW_KEY_R)
        rotate_prism == true ? rotate_prism = false : rotate_prism = true;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{

    // Flying camera
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, delta, center);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, delta, center);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, delta, center);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, delta, center);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, delta, center);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, delta, center);

    // Escape
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // Object Translation
    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
        center += camera.cameraRelative(FORWARD, delta);
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
        center += camera.cameraRelative(BACKWARD, delta);
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
        center += camera.cameraRelative(LEFT, delta);
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
        center += camera.cameraRelative(RIGHT, delta);
    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
        center += camera.cameraRelative(UP, delta);
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
        center += camera.cameraRelative(DOWN, delta);
}

int main(int argc, char *argv[])
{
    // Error check
    if (argc != 2)
    {
        cout << "ERROR: Invalid number of arguments\n";
        return 1;
    }

    int input = atoi(argv[1]);
    if (input <= 2)
    {
        cout << "ERROR: Invalid number of sides to generate polygon\n";
        return 1;
    }

    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------

    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Prism", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // build and compile our shader program
    // ------------------------------------
    Shader ourShader("../src/vertex.shader", "../src/fragment.shader");

    // Do depth comparisons and update the depth buffer.
    glEnable(GL_DEPTH_TEST);
    double theta = (double)360 / input; // Delta angle measured from the center;

    // 4 triangles for every side => 12x vertices in the prism
    // 2 attributes of 3 elements => 6 coordinates
    float vertices[72 * input], randColour[3], randerColour[3], t = 255.0;
    theta *= (double)M_PI / 180;

    // Colours
    for (int i = 0; i < 3; i++)
    {
        randColour[i] = (rand() % 255) / t; // Generate a random float < 1.0f
        randerColour[i] = (rand() % 255) / t;
    }

    double r = 0.5f;
    // Vertices for top and the bottom polygon of the prism
    for (int i = 0; i < input; i++)
    {
        // Center vertex
        vertices[18 * i] = 0.0f;
        vertices[18 * (input + i)] = vertices[18 * i];
        vertices[18 * i + 1] = 0.0f;
        vertices[18 * (input + i) + 1] = vertices[18 * i + 1];
        vertices[18 * i + 2] = 0.5f;
        vertices[18 * (input + i) + 2] = -0.5f;
        int q = 18 * i + 6, p = 18 * i + 12;

        // Vertex 2 anticlockwise
        vertices[q] = r * cos(i * theta);
        vertices[18 * (input + i) + 6] = vertices[18 * i + 6];
        vertices[q + 1] = r * sin(i * theta);
        vertices[18 * (input + i) + 7] = vertices[18 * i + 6 + 1];
        vertices[q + 2] = 0.5f;
        vertices[18 * (input + i) + 8] = -0.5f;

        // Vertex 3
        vertices[p] = r * cos((i + 1) * theta);
        vertices[18 * (input + i) + 12] = vertices[18 * i + 12];
        vertices[p + 1] = r * sin((i + 1) * theta);
        vertices[18 * (input + i) + 13] = vertices[18 * i + 12 + 1];
        vertices[p + 2] = 0.5f;
        vertices[18 * (input + i) + 14] = -0.5f;

        // Assigning colours
        for (int j = 0; j < 3; j++)
        {
            for (int k = 0; k < 3; k++)
            {
                int m = j * 6 + k + 3;
                vertices[18 * i + m] = randColour[k];
                vertices[18 * (input + i) + m] = randerColour[k];
            }
        }
    }

    // Colour every rectangle side in each iteration
    for (int i = 0; i < input; i++)
    {
        // Randomly generated colour for each side
        float colour[3];
        for (int j = 0; j < 3; j++)
            colour[j] = (rand() % (int) t) / t;

        int n = 36 * (input + i);
        // Top and bottom vertices of the triangle
        vertices[n] = r * cos(i * theta);
        vertices[n + 6] = vertices[n];
        vertices[n + 1] = r * sin(i * theta);
        vertices[n + 7] = vertices[n + 1];
        vertices[n + 2] = 0.5f;
        vertices[n + 8] = -0.5f;

        // Front Vertex
        vertices[n + 12] = r * cos((i + 1) * theta);
        vertices[n + 13] = r * sin((i + 1) * theta);
        vertices[n + 14] = 0.5f;

        // Top and bottom vertices of the triangle
        vertices[n + 18] = r * cos((i + 1) * theta);
        vertices[n + 24] = vertices[n + 18];
        vertices[n + 19] = r * sin((i + 1) * theta);
        vertices[n + 25] = vertices[n + 1 + 18];
        vertices[n + 20] = 0.5f;
        vertices[n + 26] = -0.5f;

        // Back Vertex
        vertices[n + 30] = r * cos(i * theta);
        vertices[n + 31] = r * sin(i * theta);
        vertices[n + 32] = -0.5f;

        // Colouring vertices
        for (int j = 0; j < 3; j++)
        {
            for (int k = 0; k < 3; k++)
            {
                int z = n + 6 * j + k + 3;
                vertices[z] = colour[k];
                vertices[z + 18] = colour[k];
            }
        }
    }

    int y = 12 * input;
    unsigned int indices[y], VBO, VAO, EBO;
    for (int i = 0; i < y; i++)
        indices[i] = i;

    glGenVertexArrays(1, &VAO); // we can also generate multiple VAOs or buffers at the same time
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so it can be safely unbound later.
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0); // not really necessary as well, but beware of calls that could affect VAOs while this one is bound (like binding element buffer objects, or enabling/disabling vertex attributes)

    // uncomment this call to draw in wireframe polygons.
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glm::mat4 projection;
    projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

    // Scene is translated in the opposite direction of intended movement
    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        // Calculate required movement (based on time rather than frames)
        float current = glfwGetTime();
        delta = current - last;
        last = current;
        processInput(window);

        // Rotating the camera and prism as required
        if (rotate_prism)
            prism_angle += delta * prism_rotate_speed;

        // render
        // ------
        // Background Colour
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Black and opaque
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear colour and depth buffers
        ourShader.use();

        glm::mat4 trans = glm::mat4(1.0f);
        glm::mat4 view = camera.GetViewMatrix();

        // Object is transformed
        trans = glm::translate(trans, center);
        trans = glm::rotate(trans, prism_angle, glm::vec3(0.0f, 0.0f, 1.0f));
        trans = glm::scale(trans, glm::vec3(0.5f, 0.5f, 0.5f));

        // Values to uniform matrices
        ourShader.setMat4("transform", trans);
        ourShader.setMat4("perspective", projection);
        ourShader.setMat4("view", view);
        glBindVertexArray(VAO);

        glDrawElements(GL_TRIANGLES, sizeof(indices), GL_UNSIGNED_INT, 0);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}
