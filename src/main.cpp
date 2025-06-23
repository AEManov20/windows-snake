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
#include <filesystem>
#include <csignal>

#include "Audio/AudioOutput.h"
#include "Audio/AudioSource.h"

static void errorCallback(int error, const char *description)
{
    std::cerr << "GLFW Error (" << error << "): " << description << '\n';
}

std::string readFileToString(const std::string &path);

std::string readFileToString(const std::string &path)
{
    std::ifstream file(path, std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("could not open file");
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    file.close();

    return buffer.str();
}

std::vector<uint8_t> ReadBinaryFile(const std::string& fileName)
{
    std::ifstream file(fileName, std::ios::in | std::ios::binary | std::ios::ate);

    if (!file)
    {
        throw std::runtime_error("Could not open file " + fileName);
    }

    std::streamsize fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<uint8_t> buffer(fileSize);

    if (!file.read(reinterpret_cast<char*>(buffer.data()), fileSize))
    {
        throw std::runtime_error("Error reading file " + fileName);
    }

    file.close();

    return buffer;
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
    std::cout << '\n';

    std::cout << "GLSL version: " << epoxy_glsl_version();
    if (!epoxy_is_desktop_gl()) std::cout << " ES";
    std::cout << '\n';
}

// this initializes an environment for GLFW + OpenGL ES 3.2
bool InitGLFWEnvironment()
{
    glfwSetErrorCallback(errorCallback);

    if (glfwInit() == 0)
    {
        std::cerr << "Failed to initialize GLFW" << '\n';
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
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << '\n';
        return false;
    }

    return true;
}

bool InitSDLAudioSubsystem()
{
    if (!SDL_Init(SDL_INIT_AUDIO))
    {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << '\n';
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

bool g_ShouldQuit = false;

void SignalInterruptHandler(int _unused)
{
    g_ShouldQuit = true;
}

int main()
{
    signal(SIGINT, SignalInterruptHandler);

    std::cout << std::filesystem::current_path() << '\n';
    InitSDLAudioSubsystem();

    std::unique_ptr<AudioOutput> audioOutput = std::make_unique<SdlAudioOutput>(
        SignalSpec{
            .m_Rate = 48000,
            .m_Channels = ChannelLayout(ChannelLayoutType::STEREO)
        },
        AudioEncoding::Float32);

    std::shared_ptr<AudioSource> audioSource = std::make_shared<SourceSine>(SignalSpec{
        .m_Rate = 48000,
        .m_Channels = ChannelLayout(ChannelLayoutType::STEREO)
    }, 440.f);

    auto frame = audioSource->NextFrame();

    while (frame != std::nullopt && !g_ShouldQuit)
    {
        window->PollEvents();

        auto winResolution = window->GetDimensions();

        glClearColor(0.f, 0.f, 0.f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, winResolution.x, winResolution.y);

        shader->Bind();

        glm::lookAt(const vec<3, T, Q> &eye, const vec<3, T, Q> &center, const vec<3, T, Q> &up)
        // shader->SetUniformMatrix4x4("mvp", glm::perspective(
        //                                        glm::radians(90.f),
        //                                        static_cast<float>(winResolution.x) / static_cast<float>(
        //                                            winResolution.y),
        //                                        0.01f, 100.f) *
        //                                    // view mat
        //                                    lookAt(glm::vec3(0.f, 0.f, -3.f), glm::vec3(0.f, 0.f, 0.f),
        //                                           glm::vec3(0.f, 1.f, 0.f)) *
        //                                    // model mat
        //                                    rotate(glm::mat4(1.f), glm::radians(degrees),
        //                                           glm::vec3(0.f, 1.f, 0.f)) *
        //                                    scale(glm::mat4(1.f), glm::vec3(2.f, 2.f, 2.f)));

        shader->SetUniformMatrix4x4("mvp", glm::mat4(1.f));

        vertexArray->Bind();

        glDrawArrays(GL_TRIANGLES, 0, triangleVertices.size() / 3);

        shader->Unbind();

        window->SwapBuffers();
    }

    audioOutput.reset();

    DeinitSDLEnvironment();
    //
    // if (!InitSDLEnvironment()) return 1;
    //
    // std::vector<float> triangleVertices = {};
    //
    // constexpr size_t sampleCount = 44100;
    // float piStep = 360.f / static_cast<float>(sampleCount);
    // for (size_t i = 0; i < sampleCount; i++)
    // {
    //     triangleVertices.push_back((static_cast<float>(i) / static_cast<float>(sampleCount)) * 2.f - 1.f);
    //     triangleVertices.push_back(sinf(glm::radians(static_cast<float>(i) * piStep)));
    //     triangleVertices.push_back(0.f);
    //
    //     triangleVertices.push_back((static_cast<float>(i + 1) / static_cast<float>(sampleCount)) * 2.f - 1.f);
    //     triangleVertices.push_back(sinf(glm::radians(static_cast<float>(i + 1) * piStep)));
    //     triangleVertices.push_back(0.f);
    // }
    //
    // std::unique_ptr<Window> window = std::make_unique<SdlWindow>(1280, 720, "SdlWindow");
    // window->Bind();
    //
    // std::shared_ptr<VertexBuffer> vertexBuffer = std::make_shared<GlVertexBuffer>(
    //     triangleVertices.data(), triangleVertices.size(), BufferItemLayout{
    //         BufferElement(ShaderDataType::Float3, "position")
    //     });
    //
    // std::unique_ptr<GraphicsShader> shader = std::make_unique<GlGraphicsShader>(
    //     readFileToString("../resources/vertexShader.glsl"),
    //     readFileToString("../resources/fragmentShader.glsl"));
    //
    // std::unique_ptr<VertexArray> vertexArray = std::make_unique<GlVertexArray>();
    // vertexArray->AddVertexBuffer(vertexBuffer);
    //
    // float degrees = 0.f;
    //
    // while (!window->ShouldClose())
    // {
    //     window->PollEvents();
    //
    //     auto winResolution = window->GetDimensions();
    //
    //     glClearColor(0.f, 0.f, 0.f, 1.0f);
    //     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //     glViewport(0, 0, winResolution.x, winResolution.y);
    //
    //     shader->Bind();
    //
    //     // shader->SetUniformMatrix4x4("mvp", glm::perspective(
    //     //                                        glm::radians(90.f),
    //     //                                        static_cast<float>(winResolution.x) / static_cast<float>(
    //     //                                            winResolution.y),
    //     //                                        0.01f, 100.f) *
    //     //                                    // view mat
    //     //                                    lookAt(glm::vec3(0.f, 0.f, -3.f), glm::vec3(0.f, 0.f, 0.f),
    //     //                                           glm::vec3(0.f, 1.f, 0.f)) *
    //     //                                    // model mat
    //     //                                    rotate(glm::mat4(1.f), glm::radians(degrees),
    //     //                                           glm::vec3(0.f, 1.f, 0.f)) *
    //     //                                    scale(glm::mat4(1.f), glm::vec3(2.f, 2.f, 2.f)));
    //
    //     shader->SetUniformMatrix4x4("mvp", glm::mat4(1.f));
    //
    //     vertexArray->Bind();
    //
    //     glDrawArrays(GL_LINES, 0, triangleVertices.size() / 3);
    //
    //     shader->Unbind();
    //
    //     window->SwapBuffers();
    // }
    //
    // vertexArray.reset();
    // shader.reset();
    //
    // vertexBuffer.reset();
    //
    // window.reset(nullptr);
    //
    // DeinitSDLEnvironment();
    //
    // return 0;
}
