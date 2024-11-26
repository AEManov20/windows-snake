#define GLM_ENABLE_EXPERIMENTAL
#include <epoxy/gl.h>
#include <glm/gtx/color_space.hpp>
#include <SDL3/SDL.h>
#include <GLFW/glfw3.h>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <list>
#include <iostream>
#include <random>
#include <utility>
#include <bits/random.h>

#include "SdlWindow.h"

static void errorCallback(int error, const char *description) {
    std::cerr << "GLFW Error (" << error << "): " << description << std::endl;
}

std::string readFileToString(const std::string &path);

GLuint compileShaders(const std::string &vertexCode, const std::string &fragmentCode);

std::string readFileToString(const std::string &path) {
    std::ifstream file(path, std::ios::binary);

    if (!file.is_open())
        throw std::runtime_error("could not open file");

    std::stringstream buffer;
    buffer << file.rdbuf();

    file.close();

    return buffer.str();
}

GLuint compileShaders(const std::string &vertexCode, const std::string &fragmentCode) {
    const GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    GLint compilationResult = GL_FALSE;
    GLint infoLogLength;

    char const *vertexCodePtr = vertexCode.c_str();
    char const *fragmentCodePtr = fragmentCode.c_str();

    glShaderSource(vertexShader, 1, &vertexCodePtr, nullptr);
    glCompileShader(vertexShader);

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &compilationResult);
    glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &infoLogLength);
    if (infoLogLength > 0) {
        std::vector<char> vertexShaderErrorMessage(
            static_cast<size_t>(infoLogLength) + 1);
        glGetShaderInfoLog(vertexShader, infoLogLength, nullptr,
                           &vertexShaderErrorMessage[0]);
        std::cout << "Vertex compilation error: ";
        std::cout << vertexShaderErrorMessage.data() << std::endl;

        return 0;
    }

    glShaderSource(fragmentShader, 1, &fragmentCodePtr, nullptr);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &compilationResult);
    glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &infoLogLength);
    if (infoLogLength > 0) {
        std::vector<char> fragmentShaderErrorMessage(
            static_cast<size_t>(infoLogLength) + 1);
        glGetShaderInfoLog(fragmentShader, infoLogLength, nullptr,
                           &fragmentShaderErrorMessage[0]);
        std::cout << "Fragment shader compilation error: ";
        std::cout << fragmentShaderErrorMessage.data() << std::endl;

        return 0;
    }

    GLuint program = glCreateProgram();
    GLint linkResult;
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    glGetProgramiv(program, GL_LINK_STATUS, &linkResult);
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);
    if (infoLogLength > 0) {
        std::vector<char> programErrorMessage(static_cast<size_t>(infoLogLength) +
                                              1);
        glGetProgramInfoLog(program, infoLogLength, nullptr,
                            &programErrorMessage[0]);
        std::cout << "Program linking error: ";
        std::cout << programErrorMessage.data() << std::endl;

        return 0;
    }

    glDetachShader(program, vertexShader);
    glDetachShader(program, fragmentShader);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
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

glm::ivec2 GetScreenResolution() {
    const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

    return {mode->width, mode->height};
}

struct WindowContext {
    SdlWindow window;
    glm::vec4 clearColor;
};

void updateWindow(WindowContext &context) {
    context.window.Bind();

    glClearColor(context.clearColor.r, context.clearColor.g, context.clearColor.b, context.clearColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    context.window.SwapBuffers();
    context.window.PollEvents();

    context.window.Unbind();
}

enum class Direction {
    Up, Down, Left, Right
};

int main() {
    if (!InitSDLEnvironment()) return 1;

    {
        std::random_device rd;
        std::mt19937 gen(rd());

        constexpr int gridWidth = 16, gridHeight = 9;
        constexpr int winWidth = 120, winHeight = 120;

        std::uniform_int_distribution<> gridWidthGenerator(0, gridWidth - 1);
        std::uniform_int_distribution<> gridHeightGenerator(0, gridHeight - 1);

        const uint64_t tickIntervalMilliseconds = 200;
        Direction direction = Direction::Up;
        Direction previousDirection = Direction::Up;

        glm::ivec2 snakePosition = glm::ivec2(gridWidthGenerator(gen), gridHeightGenerator(gen));
        glm::ivec2 foodPosition = glm::ivec2(gridWidthGenerator(gen), gridHeightGenerator(gen));

        WindowContext food{
            SdlWindow(winWidth, winHeight, "Food"),
            glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
        };

        std::list<WindowContext> snake;
        snake.emplace_back(WindowContext{
            SdlWindow(winWidth, winHeight, "Snake"),
            glm::vec4(0.0f, 1.0f, 0.0f, 1.0f),
        });
        snake.emplace_back(WindowContext{
            SdlWindow(winWidth, winHeight, "Snake"),
            glm::vec4(0.0f, 1.0f, 0.0f, 1.0f),
        });
        snake.emplace_back(WindowContext{
            SdlWindow(winWidth, winHeight, "Snake"),
            glm::vec4(0.0f, 1.0f, 0.0f, 1.0f),
        });

        uint64_t timeNow = SDL_GetPerformanceCounter();
        uint64_t elapsedSinceLastTick = 0;

        bool shouldQuit = false;
        while (!shouldQuit) {
            const uint64_t timePrevious = timeNow;
            timeNow = SDL_GetPerformanceCounter();

            elapsedSinceLastTick += (timeNow - timePrevious) * 1000 / SDL_GetPerformanceFrequency();

            // update windows
            updateWindow(food);
            for (auto &snakePart: snake) {
                updateWindow(snakePart);
            }

            SDL_Event event;
            while (SDL_PeepEvents(&event, 1, SDL_GETEVENT, SDL_EVENT_KEY_DOWN, SDL_EVENT_KEY_UP)) {
                if (event.type != SDL_EVENT_KEY_DOWN) {
                    continue;
                }

                switch (event.key.scancode) {
                    case SDL_SCANCODE_ESCAPE:
                        shouldQuit = true;
                        break;
                    case SDL_SCANCODE_UP:
                        if (previousDirection == Direction::Down) break;
                        direction = Direction::Up;
                        break;
                    case SDL_SCANCODE_DOWN:
                        if (previousDirection == Direction::Up) break;
                        direction = Direction::Down;
                        break;
                    case SDL_SCANCODE_LEFT:
                        if (previousDirection == Direction::Right) break;
                        direction = Direction::Left;
                        break;
                    case SDL_SCANCODE_RIGHT:
                        if (previousDirection == Direction::Left) break;
                        direction = Direction::Right;
                        break;
                    default: ;
                }
            }

            // tick game state
            previousDirection = direction;

            if (elapsedSinceLastTick < tickIntervalMilliseconds) {
                continue;
            } else {
                elapsedSinceLastTick = 0;
            }

            glm::ivec2 directionVector;
            switch (direction) {
                case Direction::Up:
                    directionVector = glm::ivec2(0, -1);
                    break;
                case Direction::Down:
                    directionVector = glm::ivec2(0, 1);
                    break;
                case Direction::Left:
                    directionVector = glm::ivec2(-1, 0);
                    break;
                case Direction::Right:
                    directionVector = glm::ivec2(1, 0);
                    break;
            }

            auto newHeadPosition = snakePosition + directionVector;

            if (newHeadPosition.x < 0) newHeadPosition.x = gridWidth - 1;
            else if (newHeadPosition.x >= gridWidth) newHeadPosition.x = 0;

            if (newHeadPosition.y < 0) newHeadPosition.y = gridHeight - 1;
            else if (newHeadPosition.y >= gridHeight) newHeadPosition.y = 0;

            snakePosition = newHeadPosition;

            bool growFlag = snakePosition == foodPosition;

            if (snakePosition == foodPosition) {
                foodPosition = glm::ivec2(gridWidthGenerator(gen), gridHeightGenerator(gen));
            }

            food.window.SetPosition(glm::ivec2(winWidth, winHeight) * foodPosition);

            snake.emplace_back(WindowContext{
                SdlWindow(winWidth, winHeight, "Snake"),
                glm::vec4(0.0f, 1.0f, 0.0f, 1.0f),
            });
            snake.back().window.SetPosition(glm::ivec2(winWidth, winHeight) * newHeadPosition);

            if (!growFlag) snake.pop_front();
        }
    }

    // {
    //     constexpr int winWidth = 120,
    //             winHeight = 120;
    //
    //     std::vector<WindowContext> windowContexts;
    //
    //     for (auto i = 0, c = 0; i < 16; i++) {
    //         for (auto j = 0; j < 10; j++, c++) {
    //             auto win = SdlWindow(winWidth, winHeight, std::to_string(i) + "." + std::to_string(j));
    //
    //             win.SetPosition(glm::ivec2((winWidth + 0) * i, (winHeight + 0) * j));
    //             windowContexts.emplace_back(WindowContext {
    //                 std::move(win),
    //                 glm::vec4(rgbColor(glm::vec3((static_cast<float>(c) / 16.f) * 360.f, 1.f, 1.f)), 1.f),
    //             });
    //         }
    //     }
    //
    //     bool shouldQuit = false;
    //     while (!shouldQuit) {
    //         if (windowContexts.empty()) {
    //             shouldQuit = true;
    //         }
    //
    //         for (auto&[window, clearColor] : windowContexts) {
    //             window.Bind();
    //
    //             glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
    //             glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //
    //             window.SwapBuffers();
    //             window.PollEvents();
    //
    //             window.Unbind();
    //         }
    //
    //         std::vector<size_t> indicesForDeletion;
    //         for (size_t i = 0; i < windowContexts.size(); i++) {
    //             if (windowContexts[i].window.ShouldClose()) {
    //                 indicesForDeletion.push_back(i);
    //             }
    //         }
    //
    //         for (const size_t idx : indicesForDeletion) {
    //             windowContexts.erase(windowContexts.begin() + idx);
    //         }
    //     }
    // }

    DeinitSDLEnvironment();

    return 0;
}
