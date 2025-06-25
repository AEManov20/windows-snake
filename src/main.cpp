#include "CameraPerspective.h"
#include "GlGraphicsShader.h"
#include "GlVertexArray.h"
#include "GlVertexBuffer.h"
#include "SdlEventQueue.h"
#include "SdlWindow.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "Window.h"
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/quaternion_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <memory>
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
#include <csignal>


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

    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

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

    // std::cout << std::filesystem::current_path() << '\n';
    // InitSDLAudioSubsystem();

    // std::unique_ptr<AudioOutput> audioOutput = std::make_unique<SdlAudioOutput>(
    //     SignalSpec{
    //         .m_Rate = 48000,
    //         .m_Channels = ChannelLayout(ChannelLayoutType::STEREO)
    //     },
    //     AudioEncoding::Float32);

    // std::shared_ptr<AudioSource> audioSource = std::make_shared<SourceSine>(SignalSpec{
    //     .m_Rate = 48000,
    //     .m_Channels = ChannelLayout(ChannelLayoutType::STEREO)
    // }, 440.f);

    // auto frame = audioSource->NextFrame();

    // while (frame != std::nullopt && !g_ShouldQuit)
    // {
    //     audioOutput->Write(*frame);
    //     frame = audioSource->NextFrame();
    // }

    // audioOutput.reset();

    // DeinitSDLEnvironment();
    if (!InitSDLEnvironment()) return 1;

    std::vector triangleVertices = {
         1.F, 1.F, 1.F, // front top right
         1.F,-1.F, 1.F, // front bottom right
        -1.F, 1.F, 1.F, // front top left
        -1.F,-1.F, 1.F, // front bottom left
         1.F, 1.F,-1.F, // back top right
         1.F,-1.F,-1.F, // back bottom right
        -1.F, 1.F,-1.F, // back top left
        -1.F,-1.F,-1.F, // back bottom right
        // first triangle
        // 0.5F, 0.5F, 0.F, // top right
        // 0.5F, -0.5F, 0.F, // bottom right
        // -0.5F, 0.5F, 0.F, // top left
        // second triangle
        // 0.5F, -0.5F, 0.F, // bottom right
        // -0.5F, -0.5F, 0.F, // bottom left
        // -0.5F, 0.5F, 0.F // top left
    };

    std::vector triangleColors = {
        0.F, 0.F, 0.F,
        0.F, 0.F, 1.F,
        0.F, 1.F, 0.F,
        0.F, 1.F, 1.F,
        1.F, 0.F, 0.F,
        1.F, 0.F, 1.F,
        1.F, 1.F, 0.F,
        1.F, 1.F, 1.F,
    };

    std::vector<std::uint32_t> indices = {
        0, 4, 5,
        5, 1, 0,
        0, 3, 1,
        3, 2, 0,
        3, 6, 2,
        3, 7, 6,
        1, 3, 7,
        1, 5, 7,
        5, 7, 6,
        5, 4, 6,
        4, 6, 2,
        4, 0, 2
    };

    std::shared_ptr eventQueue = std::make_shared<SdlEventQueue>();
    std::unique_ptr<Window> window = std::make_unique<SdlWindow>(1920, 1080, "SdlWindow", eventQueue);

    window->Bind();

    std::shared_ptr<VertexBuffer> vertexBuffer = std::make_shared<GlVertexBuffer>(
        triangleVertices.data(), triangleVertices.size(), BufferItemLayout{
            BufferElement(ShaderDataType::Float3, "position"),
        });

    std::shared_ptr<VertexBuffer> colorBuffer = std::make_shared<GlVertexBuffer>(
        triangleColors.data(), triangleColors.size(), BufferItemLayout{
            BufferElement(ShaderDataType::Float3, "color"),
        });

    std::unique_ptr<GraphicsShader> shader = std::make_unique<GlGraphicsShader>(
        readFileToString("../resources/vertexShader.glsl"),
        readFileToString("../resources/fragmentShader.glsl"));

    std::shared_ptr<IndexBuffer> indexBuffer = std::make_shared<GlIndexBuffer>(indices.data(), indices.size());

    std::unique_ptr<VertexArray> vertexArray = std::make_unique<GlVertexArray>();
    vertexArray->AddVertexBuffer(vertexBuffer);
    vertexArray->AddVertexBuffer(colorBuffer);
    vertexArray->SetIndexBuffer(indexBuffer);

    float degrees = 0.F;

    auto winResolution = window->GetDimensions();

    CameraPerspective camera = {
        .m_FieldOfView = glm::radians(90.F),
        .m_AspectRatio = static_cast<float>(winResolution.x) / static_cast<float>(winResolution.y),
        .m_NearPlane = 0.1F,
        .m_FarPlane = 100.F,
        .m_Translation = glm::vec3(0.F, 0.F, 4.F)
    };

    while (!window->ShouldClose() && !g_ShouldQuit)
    {
        eventQueue->Poll();
        window->PollEvents();

        auto winResolution = window->GetDimensions();

        if (window->IsKeyDown(KeyCode::W))
        {
            camera.m_Translation.z -= 10.F * window->GetFrameTime();
        }
        else if (window->IsKeyDown(KeyCode::S))
        {
            camera.m_Translation.z += 10.F * window->GetFrameTime();
        }
        else if (window->IsKeyDown(KeyCode::A))
        {
            camera.m_Translation.x -= 10.F * window->GetFrameTime();
        }
        else if (window->IsKeyDown(KeyCode::D))
        {
            camera.m_Translation.x += 10.F * window->GetFrameTime();
        }

        glClearColor(0.F, 0.F, 0.F, 1.0F);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, winResolution.x, winResolution.y);
        camera.m_AspectRatio = static_cast<float>(winResolution.x) / static_cast<float>(winResolution.y);

        shader->Bind();

        // shader->SetUniformMatrix4x4("mvp", glm::perspective(
        //                                        glm::radians(90.f),
        //                                        static_cast<float>(winResolution.x) / static_cast<float>(
        //                                            winResolution.y),
        //                                        0.01f, 100.f) *
        //                                    // view mat
        //                                    lookAt(glm::vec3(0.f, 0.f, 3.f), glm::vec3(0.f, 0.f, 0.f),
        //                                           glm::vec3(0.f, 1.f, 0.f)) *
        //                                    // model mat
        //                                    rotate(glm::mat4(1.f), glm::radians(degrees),
        //                                           glm::vec3(0.f, 1.f, 0.f)) *
        //                                    scale(glm::mat4(1.f), glm::vec3(2.f, 2.f, 2.f)));
        shader->SetUniformMatrix4x4("mvp", camera.GetMVPMatrix(
            rotate(glm::mat4(1.F), glm::radians(degrees), glm::vec3(1.F, 1.F, 1.F))));

        vertexArray->Bind();

        glDrawElements(GL_TRIANGLES, indexBuffer->GetCount(), GL_UNSIGNED_INT, nullptr);

        shader->Unbind();

        window->SwapBuffers();

        degrees += .1F;
        if (degrees > 360.F)
        {
            degrees = 0.F;
        }
    }

    vertexArray.reset();
    shader.reset();

    vertexBuffer.reset();
    colorBuffer.reset();

    window.reset(nullptr);

    DeinitSDLEnvironment();

    return 0;
}
