#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include "Camera/Camera.h"
#include "Shader/ShaderProgram.h"
#include "Texture/Texture.h"
#include "Vertex/Vertex.h"

const unsigned int WIDTH {800};
const unsigned int HEIGHT {600};
unsigned int currentWidth{WIDTH};
unsigned int currentHeight{HEIGHT};

void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
void CursorCallback(GLFWwindow* window, double xposIn, double yposIn);
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void ProcessInput(GLFWwindow* window);

Vertex vertices[] = {
    // positions          // texture coords  // normals           // tangents
    {{-1.0f,  1.0f, 0.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},
    {{-1.0f, -1.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},
    {{ 1.0f,  1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},
    {{-1.0f, -1.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},
    {{ 1.0f, -1.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},
    {{ 1.0f,  1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},
};

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

float deltaTime{0.0f};
float lastTime{0.0f};

float lastCursorX{WIDTH / 2.0f};
float lastCursorY{HEIGHT / 2.0f};
bool firstCursorClick{true};

glm::vec3 lightPosition(0.0f, 1.0f, 0.5f);

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

    glfwWindowHint(GLFW_SAMPLES, 4);

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
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    if(!gladLoadGL((GLADloadfunc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glm::vec3 edge1 = vertices[1].position - vertices[0].position;
    glm::vec3 edge2 = vertices[2].position - vertices[0].position;
    glm::vec2 deltaUV1 = vertices[1].texCoords - vertices[0].texCoords;
    glm::vec2 deltaUV2 = vertices[2].texCoords - vertices[0].texCoords;
    float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

    vertices[0].tangent = f * (deltaUV2.y * edge1 - deltaUV1.y * edge2);
    vertices[1].tangent = vertices[0].tangent;
    vertices[2].tangent = vertices[0].tangent;

    edge1 = vertices[4].position - vertices[3].position;
    edge2 = vertices[5].position - vertices[3].position;
    deltaUV1 = vertices[4].texCoords - vertices[3].texCoords;
    deltaUV2 = vertices[5].texCoords - vertices[3].texCoords;
    f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

    vertices[3].tangent = f * (deltaUV2.y * edge1 - deltaUV1.y * edge2);
    vertices[4].tangent = vertices[3].tangent;
    vertices[5].tangent = vertices[3].tangent;

    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    Shader *vertexShader = new Shader("shader.vs", ShaderType::VERTEX);
    Shader *fragmentShader = new Shader("shader.fs", ShaderType::FRAGMENT);
    ShaderProgram *shaderProgram = new ShaderProgram(std::vector<Shader>{*vertexShader, *fragmentShader});

    delete vertexShader;
    delete fragmentShader;

    Texture *diffuseTexture = new Texture("resources/textures/bricks2.jpg", TextureType::DIFFUSE, GL_TEXTURE0);
    Texture *normalTexture = new Texture("resources/textures/bricks2_normal.jpg", TextureType::NORMAL, GL_TEXTURE1);
    Texture *depthTexture = new Texture("resources/textures/bricks2_disp.jpg", TextureType::DISPLACEMENT, GL_TEXTURE2);

    shaderProgram->Bind();
    diffuseTexture->SetShaderUniform(*shaderProgram, "diffuseTexture");
    normalTexture->SetShaderUniform(*shaderProgram, "normalTexture");
    depthTexture->SetShaderUniform(*shaderProgram, "depthTexture");

    shaderProgram->SetVec3("lightPosition", lightPosition);

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_FRAMEBUFFER_SRGB);
    glEnable(GL_CULL_FACE);

    while(!glfwWindowShouldClose(window)) {
        float currentTime = static_cast<float>(glfwGetTime());
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        ProcessInput(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shaderProgram->Bind();
        shaderProgram->SetMat4("view", camera.GetViewMatrix());

        glm::mat4 projection = glm::perspective(glm::radians(camera.zoom), static_cast<float>(currentWidth) / currentHeight, 0.1f, 100.0f);
        shaderProgram->SetMat4("projection", projection);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, currentTime * glm::radians(-10.0f), glm::vec3(1.0f, 0.0f, 1.0f));
        shaderProgram->SetMat4("model", model);

        shaderProgram->SetVec3("viewPosition", camera.position);

        diffuseTexture->Bind();
        normalTexture->Bind();
        depthTexture->Bind();

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, sizeof(vertices) / sizeof(Vertex));
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);

    delete shaderProgram;

    delete diffuseTexture;
    delete normalTexture;
    delete depthTexture;

    glfwTerminate();
    return 0;
}

void ProcessInput(GLFWwindow* window) {
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