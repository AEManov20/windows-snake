#ifndef GLFWWINDOW_H
#define GLFWWINDOW_H
#include <string>

#include "Window.h"
#include "GLFW/glfw3.h"


class GlfwWindow final : public Window
{
public:
    GlfwWindow(int width, int height, const std::string &title);

    GlfwWindow(GlfwWindow &&other) noexcept;

    GlfwWindow &operator=(GlfwWindow &&other) noexcept;

    GlfwWindow(const GlfwWindow &other) = delete;

    GlfwWindow &operator=(const GlfwWindow &) = delete;

    bool ShouldClose() override;

    void SwapBuffers() override;

    void PollEvents() override;

    void SetPosition(glm::ivec2 position) override;

    glm::ivec2 GetPosition() override;

    void Bind() override;

    void Unbind() override;

    glm::ivec2 GetDimensions() override;

    ~GlfwWindow() override;

private:
    GLFWwindow *m_Handle;
    std::string m_WindowTitle;
};


#endif //GLFWWINDOW_H
