#define GLM_ENABLE_EXPERIMENTAL
#include <epoxy/gl.h>
#include <glm/gtx/color_space.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <SDL3/SDL.h>
#include <GLFW/glfw3.h>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>
#include <random>
#include <array>
#include <filesystem>

#include "GlfwWindow.h"
#include "GlslGraphicsShader.h"
#include "SdlWindow.h"

static void errorCallback(int error, const char *description) {
    std::cerr << "GLFW Error (" << error << "): " << description << std::endl;
}

std::string readFileToString(const std::string &path);

std::string readFileToString(const std::string &path) {
    std::ifstream file(path, std::ios::binary);

    if (!file.is_open())
        throw std::runtime_error("could not open file");

    std::stringstream buffer;
    buffer << file.rdbuf();

    file.close();

    return buffer.str();
}

GLuint createBufferFromData(const GLvoid *data, const GLsizeiptr size) {
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return buffer;
}

void freeBufferData(const GLuint buffer) {
    glDeleteBuffers(1, &buffer);
}

void PrintGLVersion() {
    std::cout << "GL version: " << epoxy_gl_version();
    if (!epoxy_is_desktop_gl()) std::cout << " ES";
    std::cout << std::endl;

    std::cout << "GLSL version: " << epoxy_glsl_version();
    if (!epoxy_is_desktop_gl()) std::cout << " ES";
    std::cout << std::endl;
}

// this initializes an environment for GLFW + OpenGL ES 3.2
bool InitGLFWEnvironment() {
    glfwSetErrorCallback(errorCallback);

    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return false;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_SAMPLES, 4);
    return true;
}

// this initializes an environment for SDL + OpenGL ES 3.2
bool InitSDLEnvironment() {
    // request OpenGL ES 3.2
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

    // request double-buffering
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    // init SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
        return false;
    }

    return true;
}

void DeinitGLFWEnvironment() {
    glfwTerminate();
}

void DeinitSDLEnvironment() {
    SDL_Quit();
}

int main() {
    std::cout << std::filesystem::current_path() << std::endl;

    if (!InitGLFWEnvironment()) return 1;

    constexpr std::array triangleUV = {
        0.f, 0.f,
        1.f, 0.f,
        .5f, 1.f,
    };

    constexpr std::array triangleVertices = {
        -1.f, -1.f, 0.f,
         1.f, -1.f, 0.f,
         0.f,  1.f, 0.f,
    };

    constexpr std::array triangleColors = {
        1.f, 0.f, 0.f,
        0.f, 1.f, 0.f,
        0.f, 0.f, 1.f,
    };

    {
        GlfwWindow window(1280, 720, "GlfwWindow");
        window.Bind();

        const GLuint uvBuffer = createBufferFromData(triangleUV.data(), triangleUV.size() * sizeof(triangleUV[0]));
        const GLuint vertexBuffer = createBufferFromData(triangleVertices.data(),
            triangleVertices.size() * sizeof(triangleVertices[0]));
        const GLuint colorBuffer = createBufferFromData(triangleColors.data(),
            triangleColors.size() * sizeof(triangleColors[0]));

        {
            GlslGraphicsShader shader(readFileToString("../resources/vertexShader.glsl"),
                readFileToString("../resources/fragmentShader.glsl"));

            const GLuint program = shader.GetProgramID();

            const GLint positionAttribLoc = glGetAttribLocation(program, "position");
            const GLint uvAttribLoc = glGetAttribLocation(program, "uv");
            const GLint colorAttribLoc = glGetAttribLocation(program, "color");

            float degrees = 0.f;

            while (!window.ShouldClose()) {
                window.PollEvents();

                auto winResolution = window.GetDimensions();

                glClearColor(0.f, 0.f, 0.f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                glViewport(0, 0, winResolution.x, winResolution.y);

                shader.Bind();

                shader.SetUniformMatrix4x4("mvp", glm::perspective(
                    glm::radians(90.f),
                    static_cast<float>(winResolution.x) / static_cast<float>(winResolution.y),
                    0.01f, 100.f) *
                    // view mat
                    lookAt(glm::vec3(0.f, 0.f, -6.f), glm::vec3(0.f, 0.f, 0.f),
                                glm::vec3(0.f, 1.f, 0.f)) *
                    // model mat
                    rotate(glm::mat4(1.f), glm::radians(degrees), glm::vec3(0.f, 1.f, 0.f)) *
                    scale(glm::mat4(1.f), glm::vec3(2.f, 2.f, 2.f)));

                degrees += 1.f;
                if (degrees >= 360.f)
                    degrees -= 360.f;

                glEnableVertexAttribArray(positionAttribLoc);
                glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
                glVertexAttribPointer(positionAttribLoc, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

                glEnableVertexAttribArray(uvAttribLoc);
                glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
                glVertexAttribPointer(uvAttribLoc, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

                glEnableVertexAttribArray(colorAttribLoc);
                glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
                glVertexAttribPointer(colorAttribLoc, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

                glBindBuffer(GL_ARRAY_BUFFER, 0);
                glDrawArrays(GL_TRIANGLES, 0, 3);

                glDisableVertexAttribArray(positionAttribLoc);
                glDisableVertexAttribArray(uvAttribLoc);
                glDisableVertexAttribArray(colorAttribLoc);

                shader.Unbind();

                window.SwapBuffers();
            }
        }

        freeBufferData(uvBuffer);
        freeBufferData(vertexBuffer);
        freeBufferData(colorBuffer);
    }


    DeinitGLFWEnvironment();

    return 0;
}
