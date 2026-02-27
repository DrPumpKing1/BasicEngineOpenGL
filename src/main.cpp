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

    unsigned int instances = 5000;
    glm::vec3 *positions = new glm::vec3[instances];
    srand(glfwGetTime()); // initialize random seed
    float radius = 2.5f;
    float offset = 0.5f;
    for (unsigned int i = 0; i < instances; i++)
    {
        float angle = (float)i / (float)instances * 360.0f;
        float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float x = sin(angle) * radius + displacement;
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float y = displacement * 0.4f; // keep height of field smaller compared to width of x and z
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float z = cos(angle) * radius + displacement;

        positions[i] = glm::vec3(x, y, z);
    }

    ParticlesLit *particlesLit = new ParticlesLit(instances);
    particlesLit->AddProperties(3, positions);

    std::shared_ptr<Shader> vertexShader, fragmentShader;

    std::shared_ptr<Texture> particleTexture;
    ResourceManager::Instance().NewShaderAsset("shaders/particleLit.vs", ShaderType::VERTEX, vertexShader);
    ResourceManager::Instance().NewShaderAsset("shaders/particleLit.fs", ShaderType::FRAGMENT, fragmentShader);

    ShaderProgram *shaderProgram = new ShaderProgram();
    shaderProgram->AttachShader(vertexShader);
    shaderProgram->AttachShader(fragmentShader);
    shaderProgram->Compile();

    vertexShader.reset();
    fragmentShader.reset();

    ResourceManager::Instance().NewTextureAsset("textures/bricks2.jpg", TextureType::DIFFUSE, GL_TEXTURE0, false, particleTexture);

    UniformBlock *cameraUniformBlock = new UniformBlock("Camera", 0, sizeof(CameraData));
    cameraUniformBlock->SetShaderUniformBlock(*shaderProgram);
    struct CameraData cameraData;

    shaderProgram->Bind();
    shaderProgram->SetFloat("scale", 0.5f);
    shaderProgram->SetVec2("offset", glm::vec2(0.0f, 0.0f));
    particleTexture->SetShaderUniform(*shaderProgram, "diffuseTexture");

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_FRAMEBUFFER_SRGB);

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

        shaderProgram->Bind();
        shaderProgram->SetFloat("time", currentTime);
        float radius = 0.05f + sin(currentTime) * 0.025f;
        shaderProgram->SetFloat("radius", radius);

        particleTexture->Bind();

        particlesLit->Draw(instances);

        particleTexture->Unbind();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    delete particlesLit;

    delete shaderProgram;

    delete cameraUniformBlock;

    particleTexture.reset();

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