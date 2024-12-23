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
#include <filesystem>

#include "GlfwWindow.h"
#include "GlGraphicsShader.h"
#include "GlVertexArray.h"
#include "GlVertexBuffer.h"
#include "SdlWindow.h"
#include "VertexArray.h"

static void errorCallback(int error, const char *description)
{
    std::cerr << "GLFW Error (" << error << "): " << description << std::endl;
}

std::string readFileToString(const std::string &path);

std::string readFileToString(const std::string &path)
{
    std::ifstream file(path, std::ios::binary);

    if (!file.is_open())
        throw std::runtime_error("could not open file");

    std::stringstream buffer;
    buffer << file.rdbuf();

    file.close();

    return buffer.str();
}

GLuint createBufferFromData(const GLvoid *data, const GLsizeiptr size)
{
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return buffer;
}

void freeBufferData(const GLuint buffer)
{
    glDeleteBuffers(1, &buffer);
}

void PrintGLVersion()
{
    std::cout << "GL version: " << epoxy_gl_version();
    if (!epoxy_is_desktop_gl()) std::cout << " ES";
    std::cout << std::endl;

    std::cout << "GLSL version: " << epoxy_glsl_version();
    if (!epoxy_is_desktop_gl()) std::cout << " ES";
    std::cout << std::endl;
}

// this initializes an environment for GLFW + OpenGL ES 3.2
bool InitGLFWEnvironment()
{
    glfwSetErrorCallback(errorCallback);

    if (!glfwInit())
    {
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
bool InitSDLEnvironment()
{
    // request OpenGL ES 3.2
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

    // request double-buffering
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    // init SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
        return false;
    }

    return true;
}

void DeinitGLFWEnvironment()
{
    glfwTerminate();
}

void DeinitSDLEnvironment()
{
    SDL_Quit();
}

int main()
{
    std::cout << std::filesystem::current_path() << std::endl;

    if (!InitSDLEnvironment()) return 1;

    std::vector triangleVertices = {
        // first triangle
         0.5f,  0.5f, 0.f,  // top right
         0.5f, -0.5f, 0.f,  // bottom right
        -0.5f,  0.5f, 0.f,  // top left
        // second triangle
         0.5f, -0.5f, 0.f,  // bottom right
        -0.5f, -0.5f, 0.f,  // bottom left
        -0.5f,  0.5f, 0.f   // top left
    };

    std::vector triangleColors = {
        1.f, 0.f, 0.f,
        0.f, 1.f, 0.f,
        0.f, 0.f, 1.f,
        0.f, 0.f, 1.f,
        0.f, 1.f, 0.f,
        1.f, 0.f, 0.f,
    };

    std::vector<uint32_t> indices = { 0, 1, 2, 3, 4, 5 };

    std::unique_ptr<Window> window = std::make_unique<SdlWindow>(1280, 720, "SdlWindow");
    window->Bind();

    glEnable(GL_DEPTH_TEST);

    std::shared_ptr<VertexBuffer> vertexBuffer = std::make_shared<GlVertexBuffer>(triangleVertices.data(), triangleVertices.size(), BufferItemLayout{
        BufferElement(ShaderDataType::Float3, "position")
    });

    std::shared_ptr<IndexBuffer> indexBuffer = std::make_shared<GlIndexBuffer>(indices.data(), indices.size());

    std::shared_ptr<VertexBuffer> colorBuffer = std::make_shared<GlVertexBuffer>(triangleColors.data(), triangleColors.size(), BufferItemLayout{
        BufferElement(ShaderDataType::Float3, "color")
    });

    std::unique_ptr<GraphicsShader> shader = std::make_unique<GlGraphicsShader>(readFileToString("../resources/vertexShader.glsl"),
                            readFileToString("../resources/fragmentShader.glsl"));

    std::unique_ptr<VertexArray> vertexArray = std::make_unique<GlVertexArray>();
    vertexArray->AddVertexBuffer(vertexBuffer);
    vertexArray->AddVertexBuffer(colorBuffer);
    vertexArray->SetIndexBuffer(indexBuffer);

    float degrees = 0.f;

    while (!window->ShouldClose())
    {
        window->PollEvents();

        auto winResolution = window->GetDimensions();

        glClearColor(0.f, 0.f, 0.f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, winResolution.x, winResolution.y);

        shader->Bind();

        shader->SetUniformMatrix4x4("mvp", glm::perspective(
                                              glm::radians(90.f),
                                              static_cast<float>(winResolution.x) / static_cast<float>(
                                                  winResolution.y),
                                              0.01f, 100.f) *
                                          // view mat
                                          lookAt(glm::vec3(0.f, 0.f, -3.f), glm::vec3(0.f, 0.f, 0.f),
                                                 glm::vec3(0.f, 1.f, 0.f)) *
                                          // model mat
                                          rotate(glm::mat4(1.f), glm::radians(degrees),
                                                 glm::vec3(0.f, 1.f, 0.f)) *
                                          scale(glm::mat4(1.f), glm::vec3(2.f, 2.f, 2.f)));

        degrees += 1.f;
        if (degrees >= 360.f)
            degrees -= 360.f;

        // glEnableVertexAttribArray(positionAttribLoc);
        // vertexBuffer->Bind();
        // glVertexAttribPointer(positionAttribLoc, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        //
        // glEnableVertexAttribArray(colorAttribLoc);
        // colorBuffer->Bind();
        // glVertexAttribPointer(colorAttribLoc, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        //
        // glBindBuffer(GL_ARRAY_BUFFER, 0);
        // glDrawArrays(GL_TRIANGLES, 0, 6);
        //
        // glDisableVertexAttribArray(positionAttribLoc);
        // glDisableVertexAttribArray(colorAttribLoc);
        vertexArray->Bind();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

        shader->Unbind();

        window->SwapBuffers();
    }

    vertexArray.reset();
    shader.reset();

    vertexBuffer.reset();
    colorBuffer.reset();

    window.reset(nullptr);

    DeinitSDLEnvironment();

    return 0;
}
