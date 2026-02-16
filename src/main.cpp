#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include "Camera/Camera.h"
#include "Shader/ShaderProgram.h"
#include "Texture/Texture.h"

const unsigned int WIDTH {800};
const unsigned int HEIGHT {600};
unsigned int currentWidth{WIDTH};
unsigned int currentHeight{HEIGHT};

void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
void CursorCallback(GLFWwindow* window, double xposIn, double yposIn);
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void ProcessInput(GLFWwindow* window);

float quadVertices[] = {
    // positions        // texture Coords
    -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
    -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
     1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
     1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
};

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

float deltaTime{0.0f};
float lastTime{0.0f};

float lastCursorX{WIDTH / 2.0f};
float lastCursorY{HEIGHT / 2.0f};
bool firstCursorClick{true};

int main( void)
{
    if(!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow *window = glfwCreateWindow(currentWidth, currentHeight, "Basic OpenGL Window", nullptr, nullptr);
    if(window == nullptr) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
    glfwSetCursorPosCallback(window, CursorCallback);
    glfwSetScrollCallback(window, ScrollCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if(!gladLoadGL((GLADloadfunc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(sizeof(float) * 3));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    Shader vertexShader("shader.vs", ShaderType::VERTEX);
    Shader fragmentShader("shader.fs", ShaderType::FRAGMENT);
    ShaderProgram shaderProgram(std::vector<Shader>{vertexShader, fragmentShader});

    Texture diffuseTexture("resources/textures/brickwall.jpg", TextureType::DIFFUSE, GL_TEXTURE0);
    Texture normalTexture("resources/textures/brickwall_normal.jpg", TextureType::NORMAL, GL_TEXTURE1);

    shaderProgram.Bind();
    diffuseTexture.SetShaderUniform(shaderProgram, "diffuseTexture");
    normalTexture.SetShaderUniform(shaderProgram, "normalTexture");

    while(!glfwWindowShouldClose(window)) {
        float currentTime = static_cast<float>(glfwGetTime());
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        ProcessInput(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shaderProgram.Bind();
        shaderProgram.SetMat4("view", camera.GetViewMatrix());

        glm::mat4 projection = glm::perspective(glm::radians(camera.zoom), static_cast<float>(currentWidth) / currentHeight, 0.1f, 100.0f);
        shaderProgram.SetMat4("projection", projection);

        glm::mat4 model = glm::mat4(1.0f);
        shaderProgram.SetMat4("model", model);

        diffuseTexture.Bind();
        normalTexture.Bind();

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void ProcessInput(GLFWwindow* window) {
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        camera.ProcessKeyboardSpeed(true);
    else
        camera.ProcessKeyboardSpeed(false);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboardMovement(CameraMovement::FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboardMovement(CameraMovement::BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboardMovement(CameraMovement::LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboardMovement(CameraMovement::RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        camera.ProcessKeyboardMovement(CameraMovement::UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        camera.ProcessKeyboardMovement(CameraMovement::DOWN, deltaTime);
}

void FramebufferSizeCallback(GLFWwindow* window, int width, int height) {
    currentWidth = width;
    currentHeight = height;
    glViewport(0, 0, width, height);
}

void CursorCallback(GLFWwindow* window, double xposIn, double yposIn) {
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstCursorClick)
    {
        lastCursorX = xpos;
        lastCursorY = ypos;
        firstCursorClick = false;
    }

    float xoffset = xpos - lastCursorX;
    float yoffset = lastCursorY - ypos;

    lastCursorX = xpos;
    lastCursorY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}