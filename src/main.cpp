#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

const unsigned int WIDTH {800};
const unsigned int HEIGHT {600};

void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
void CursorCallback(GLFWwindow* window, double xpos, double ypos);
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void ProcessInput(GLFWwindow* window);

float quadVertices[]{
    // position      //tex coords
    -1.0f, 1.0f, 0.0f, 1.0f,  // top left
    -1.0f, -1.0f, 0.0f, 0.0f, // bottom left
    1.0f, -1.0f, 1.0f, 0.0f,  // bottom right
    1.0f, 1.0f, 1.0f, 1.0f    // top right
};

int main( void)
{
    unsigned int currentWidth {WIDTH};
    unsigned int currentHeight {HEIGHT};

    float deltaTime {0.0f};
    float time {0.0f};

    float lastCursorX {WIDTH / 2.0f};
    float lastCursorY {HEIGHT / 2.0f};
    bool firstMouseClick {true};

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

    while(!glfwWindowShouldClose(window)) {
        float currentTime = static_cast<float>(glfwGetTime());
        deltaTime = currentTime - time;
        time = currentTime;

        ProcessInput(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}