#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include "Camera/Camera.h"
#include "Shader/ShaderProgram.h"
#include "Texture/Texture.h"
#include "Plane/Plane3DTangent.h"
#include "Plane/Plane2D.h"
#include "Framebuffer/Framebuffer.h"

Framebuffer* FBOms;
Framebuffer* resolveFBO;
Framebuffer* resolveBlurFBO;
Framebuffer* PingpongFBOs;

const unsigned int WIDTH {800};
const unsigned int HEIGHT {600};
unsigned int currentWidth{WIDTH};
unsigned int currentHeight{HEIGHT};

void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
void CursorCallback(GLFWwindow* window, double xposIn, double yposIn);
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void ProcessInput(GLFWwindow* window);

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

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

    FBOms = new Framebuffer(true, true, 2, currentWidth, currentHeight);
    PingpongFBOs = new Framebuffer[2] {
        Framebuffer(false, false, 1, currentWidth, currentHeight),
        Framebuffer(false, false, 1, currentWidth, currentHeight)
    };
    resolveFBO = new Framebuffer(false, false, 1, currentWidth, currentHeight);
    resolveBlurFBO = new Framebuffer(false, false, 1, currentWidth, currentHeight);

    Plane2D* quadPlane2D = new Plane2D();
    Plane3DTangent* quadPlane = new Plane3DTangent();

    Shader *vertexShader = new Shader("plane3Dtangent.vs", ShaderType::VERTEX);
    Shader *fragmentShader = new Shader("plane3DtangentBloom.fs", ShaderType::FRAGMENT);
    Shader *quadVertex = new Shader("quad.vs", ShaderType::VERTEX);
    Shader *quadHDRFragment = new Shader("quadHDR.fs", ShaderType::FRAGMENT);
    Shader *guassianBlurFragment = new Shader("gaussianBlur.fs", ShaderType::FRAGMENT);

    ShaderProgram *shaderProgram = new ShaderProgram(std::vector<Shader>{*vertexShader, *fragmentShader});
    ShaderProgram *quadShaderProgram = new ShaderProgram(std::vector<Shader>{*quadVertex, *quadHDRFragment});
    ShaderProgram *gaussianBlurProgram = new ShaderProgram(std::vector<Shader>{*quadVertex, *guassianBlurFragment});

    delete vertexShader;
    delete fragmentShader;
    delete quadVertex;
    delete quadHDRFragment;
    delete guassianBlurFragment;

    Texture *diffuseTexture = new Texture("resources/textures/bricks2.jpg", TextureType::DIFFUSE, GL_TEXTURE0);
    Texture *normalTexture = new Texture("resources/textures/bricks2_normal.jpg", TextureType::NORMAL, GL_TEXTURE1);
    Texture *depthTexture = new Texture("resources/textures/bricks2_disp.jpg", TextureType::DISPLACEMENT, GL_TEXTURE2);

    shaderProgram->Bind();
    diffuseTexture->SetShaderUniform(*shaderProgram, "diffuseTexture");
    normalTexture->SetShaderUniform(*shaderProgram, "normalTexture");
    depthTexture->SetShaderUniform(*shaderProgram, "depthTexture");
    shaderProgram->SetVec3("lightPosition", lightPosition);

    quadShaderProgram->Bind();
    quadShaderProgram->SetInt("colorTexture", 0);
    quadShaderProgram->SetInt("bloomTexture", 1);

    gaussianBlurProgram->Bind();
    gaussianBlurProgram->SetInt("sourceTexture", 0);

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    while(!glfwWindowShouldClose(window)) {
        float currentTime = static_cast<float>(glfwGetTime());
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        ProcessInput(window);

        glEnable(GL_DEPTH_TEST);
        FBOms->Bind();
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shaderProgram->Bind();
        shaderProgram->SetMat4("view", camera.GetViewMatrix());

        glm::mat4 projection = glm::perspective(glm::radians(camera.zoom), static_cast<float>(currentWidth) / currentHeight, 0.1f, 100.0f);
        shaderProgram->SetMat4("projection", projection);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, currentTime * glm::radians(-10.0f), glm::vec3(1.0f, 0.0f, 1.0f));
        shaderProgram->SetMat4("model", model);

        shaderProgram->SetVec3("viewPosition", camera.position);

        float bloom = 0.75f + sin(currentTime) * 0.25f;
        shaderProgram->SetFloat("bloom", bloom);

        diffuseTexture->Bind();
        normalTexture->Bind();
        depthTexture->Bind();

        quadPlane->Bind();
        quadPlane->Draw();
        quadPlane->Unbind();

        diffuseTexture->Unbind();
        normalTexture->Unbind();
        depthTexture->Unbind();

        glDisable(GL_DEPTH_TEST);

        BlitFramebuffer(*FBOms, *resolveFBO, GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT0);
        BlitFramebuffer(*FBOms, *resolveBlurFBO, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT0);

        bool horizontal = true;
        bool firstIteration = true;
        unsigned int pingpong = 10;
        gaussianBlurProgram->Bind();
        quadPlane2D->Bind();
        for (unsigned int i = 0; i < pingpong; i++)
        {
            PingpongFBOs[horizontal].Bind();
            glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
            gaussianBlurProgram->SetBool("horizontal", horizontal);
            if(firstIteration) {
                resolveBlurFBO->BindTexture(0, GL_TEXTURE0);
                firstIteration = false;
            } else {
                PingpongFBOs[!horizontal].BindTexture(0, GL_TEXTURE0);
            }
            quadPlane2D->Draw();
            horizontal = !horizontal;
        }
        quadPlane2D->Unbind();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT);

        quadShaderProgram->Bind();
        float exposure = 0.75f + sin(currentTime) * 0.25f;
        quadShaderProgram->SetFloat("exposure", exposure);

        resolveFBO->BindTexture(0, GL_TEXTURE0);
        PingpongFBOs[!horizontal].BindTexture(0, GL_TEXTURE1);
        resolveBlurFBO->BindTexture(0, GL_TEXTURE1);
        quadPlane2D->Bind();
        quadPlane2D->Draw();
        quadPlane2D->Unbind();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    delete quadPlane2D;
    delete quadPlane;

    delete shaderProgram;
    delete quadShaderProgram;
    delete gaussianBlurProgram;

    delete diffuseTexture;
    delete normalTexture;
    delete depthTexture;

    delete FBOms;
    delete resolveFBO;
    delete resolveBlurFBO;
    delete[] PingpongFBOs;
    PingpongFBOs = nullptr;

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
    FBOms->Resize(width, height);
    resolveFBO->Resize(width, height);
    resolveBlurFBO->Resize(width, height);
    PingpongFBOs[0].Resize(width, height);
    PingpongFBOs[1].Resize(width, height);
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