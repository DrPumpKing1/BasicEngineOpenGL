#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include "Engine/Engine.h"

const unsigned int WIDTH {800};
const unsigned int HEIGHT {600};
unsigned int currentWidth{WIDTH};
unsigned int currentHeight{HEIGHT};

void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
void CursorCallback(GLFWwindow* window, double xposIn, double yposIn);
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void ProcessInput(GLFWwindow* window);

Camera camera(glm::vec3(0.0f, 0.0f, 6.0f));

float deltaTime{0.0f};
float lastTime{0.0f};

float lastCursorX{WIDTH / 2.0f};
float lastCursorY{HEIGHT / 2.0f};
bool firstCursorClick{true};
bool cursorInGame {false};

glm::vec3 lightPosition(0.0f, 2.5f, 0.0f);

std::vector<std::string> faces {
    "right.png",
    "left.png",
    "top.png",
    "bottom.png",
    "front.png",
    "back.png",
};

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
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    if(!gladLoadGL((GLADloadfunc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    std::shared_ptr<Model> backpack;
    ResourceManager::Instance().NewModelAsset("objects/backpack/backpack.obj", backpack);

    Cube *cube = new Cube();

    std::shared_ptr<Cubemap> skybox;
    ResourceManager::Instance().NewCubemapAsset("cubemaps/Galaxy", faces, GL_TEXTURE0, skybox);

    std::shared_ptr<Shader> skyboxVertexShader, skyboxFragmentShader;

    ResourceManager::Instance().NewShaderAsset("shaders/skybox.vs", ShaderType::VERTEX, skyboxVertexShader);
    ResourceManager::Instance().NewShaderAsset("shaders/skybox.fs", ShaderType::FRAGMENT, skyboxFragmentShader);

    ShaderProgram *skyboxShaderProgram = new ShaderProgram();
    skyboxShaderProgram->AttachShader(skyboxVertexShader);
    skyboxShaderProgram->AttachShader(skyboxFragmentShader);
    skyboxShaderProgram->Compile();

    skyboxVertexShader.reset();
    skyboxFragmentShader.reset();

    std::shared_ptr<Shader> vertexShader, fragmentShader;

    ResourceManager::Instance().NewShaderAsset("shaders/plane3Dtangent.vs", ShaderType::VERTEX, vertexShader);
    ResourceManager::Instance().NewShaderAsset("shaders/model3D.fs", ShaderType::FRAGMENT, fragmentShader);

    ShaderProgram *shaderProgram = new ShaderProgram();
    shaderProgram->AttachShader(vertexShader);
    shaderProgram->AttachShader(fragmentShader);
    shaderProgram->Compile();

    vertexShader.reset();
    fragmentShader.reset();

    UniformBlock *cameraUniformBlock = new UniformBlock("Camera", 0, sizeof(CameraData));
    cameraUniformBlock->SetShaderUniformBlock(*skyboxShaderProgram);
    cameraUniformBlock->SetShaderUniformBlock(*shaderProgram);
    struct CameraData cameraData;

    skyboxShaderProgram->Bind();
    skybox->SetShaderUniform(*skyboxShaderProgram, "skyboxTexture");

    shaderProgram->Bind();
    shaderProgram->SetVec3("lightPosition", lightPosition);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_FRAMEBUFFER_SRGB);

    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    while(!glfwWindowShouldClose(window)) {
        float currentTime = static_cast<float>(glfwGetTime());
        deltaTime = currentTime - lastTime;
        float fps = 1.0f / deltaTime;
        lastTime = currentTime;

        glfwSetWindowTitle(window, ("Basic OpenGL Engine - FPS: " + std::to_string(fps)).c_str());
        ProcessInput(window);

        cameraData.view = camera.GetViewMatrix();
        cameraData.projection = glm::perspective(glm::radians(camera.zoom), static_cast<float>(currentWidth) / currentHeight, 0.1f, 100.0f);
        cameraData.viewPosition = glm::vec3(camera.position);
        cameraUniformBlock->UpdateData(&cameraData);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 model(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        shaderProgram->Bind();
        shaderProgram->SetMat4("model", model);

        backpack->Draw(*shaderProgram);

        glDepthMask(GL_FALSE);
        glDepthFunc(GL_LEQUAL);
        skyboxShaderProgram->Bind();
        skybox->Bind();
        cube->Draw();
        skybox->Unbind();
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    delete cube;

    delete skyboxShaderProgram;
    delete shaderProgram;

    delete cameraUniformBlock;

    backpack.reset();

    ResourceManager::Instance().Release();

    glfwTerminate();
    return 0;
}

void ProcessInput(GLFWwindow* window) {
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS && cursorInGame) {
        cursorInGame = false;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
    
    if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && !cursorInGame) {
        cursorInGame = true;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }

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
    if(!cursorInGame) {
        firstCursorClick = true;
        return;
    }

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