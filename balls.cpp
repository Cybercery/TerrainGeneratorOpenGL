#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <iostream>

#define STB_PERLIN_IMPLEMENTATION
#include "stb_perlin.h"

#include "shader.h"

const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

const int SIZE = 200;
const float NOISE_SCALE = 0.05f;
const float HEIGHT_SCALE = 10.0f;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

int   OCTAVES = 6;
float INITIAL_AMPLITUDE = 1.0f;
float INITIAL_FREQUENCY = 0.5f;
float PERSISTENCE = 0.5f;
float LACUNARITY = 2.0f;

glm::vec3 cameraPos = glm::vec3(100.0f, 40.0f, -50.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, -0.3f, 1.0f);

glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
    float speed = 20.0f * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += speed * cameraFront;

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= speed * cameraFront;

    glm::vec3 right = glm::normalize(glm::cross(cameraFront, cameraUp));

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= right * speed;

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += right * speed;
}

float fbm(int x, int z) {
    float value = 0.0f;
    float amplitude = INITIAL_AMPLITUDE;
    float frequency = INITIAL_FREQUENCY;

    for (int i = 0; i < OCTAVES; i++) {
        value += stb_perlin_noise3(
            x * NOISE_SCALE * frequency,
            z * NOISE_SCALE * frequency,
            0.0f, 0, 0, 0
        ) * amplitude;

        amplitude *= PERSISTENCE;
        frequency *= LACUNARITY;
    }

    return value * HEIGHT_SCALE;
}

int main()
{
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(
        SCR_WIDTH,
        SCR_HEIGHT,
        "Terrain",
        NULL,
        NULL
    );

    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glEnable(GL_DEPTH_TEST);

    Shader shader("../../../shaders/terrain.vert", "../../../shaders/terrain.frag");

    int success;
    char infoLog[512];
    glGetProgramiv(shader.ID, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shader.ID, 512, NULL, infoLog);
        std::cout << "Shader link error: " << infoLog << std::endl;
    }

    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    for (int z = 0; z < SIZE; z++)
    {
        for (int x = 0; x < SIZE; x++)
        {
            float height = fbm(x, z); 

            vertices.push_back((float)x);
            vertices.push_back(height);
            vertices.push_back((float)z);
        }
    }

    for (int z = 0; z < SIZE - 1; z++)
    {
        for (int x = 0; x < SIZE - 1; x++)
        {
            int topLeft = z * SIZE + x;
            int topRight = topLeft + 1;
            int bottomLeft = (z + 1) * SIZE + x;
            int bottomRight = bottomLeft + 1;

            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);

            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }
    }

    unsigned int VAO, VBO, EBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(
        GL_ARRAY_BUFFER,
        vertices.size() * sizeof(float),
        vertices.data(),
        GL_STATIC_DRAW
    );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        indices.size() * sizeof(unsigned int),
        indices.data(),
        GL_STATIC_DRAW
    );

    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        3 * sizeof(float),
        (void*)0
    );

    glEnableVertexAttribArray(0);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.5f, 0.7f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();

        glm::mat4 model = glm::mat4(1.0f);

        glm::mat4 view = glm::lookAt(
            cameraPos,
            cameraPos + cameraFront,
            cameraUp
        );

        glm::mat4 projection = glm::perspective(
            glm::radians(45.0f),
            (float)SCR_WIDTH / (float)SCR_HEIGHT,
            0.1f,
            1000.0f
        );

        shader.setMat4("model", model);
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);

        glBindVertexArray(VAO);

        glDrawElements(
            GL_TRIANGLES,
            indices.size(),
            GL_UNSIGNED_INT,
            0
        );

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glfwTerminate();

    return 0;
}