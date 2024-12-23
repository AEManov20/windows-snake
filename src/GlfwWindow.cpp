//
// Created by alex on 24.11.24.
//

#include "GlfwWindow.h"

#include <iostream>
#include <utility>
#include <GLFW/glfw3.h>

GlfwWindow::GlfwWindow(const int width, const int height, const std::string &title)
{
    m_WindowTitle = title;

    // window creation stage
    m_Handle = glfwCreateWindow(width, height, m_WindowTitle.c_str(), nullptr, nullptr);
    if (!m_Handle)
    {
        throw std::runtime_error("Failed to create GLFW window");
    }

    // setting swap interval stage
    // store currently set glfw context
    GLFWwindow *currentContext = glfwGetCurrentContext();

    // make the instance window the current context
    glfwMakeContextCurrent(m_Handle);

    // set the swap interval
    glfwSwapInterval(1);

    // reset the context back to normal
    glfwMakeContextCurrent(currentContext);
}

GlfwWindow::GlfwWindow(GlfwWindow &&other) noexcept : m_Handle(std::exchange(other.m_Handle, nullptr)),
                                                      m_WindowTitle(std::move(other.m_WindowTitle))
{
}

GlfwWindow &GlfwWindow::operator=(GlfwWindow &&other) noexcept
{
    GlfwWindow temp(std::move(other));
    std::swap(m_Handle, temp.m_Handle);
    std::swap(m_WindowTitle, temp.m_WindowTitle);

    return *this;
}

bool GlfwWindow::ShouldClose()
{
    return glfwWindowShouldClose(m_Handle);
}

void GlfwWindow::SwapBuffers()
{
    glfwSwapBuffers(m_Handle);
}

void GlfwWindow::PollEvents()
{
    GLFWwindow *currentContext = glfwGetCurrentContext();
    if (currentContext != m_Handle)
    {
        glfwMakeContextCurrent(m_Handle);
    }

    glfwPollEvents();

    if (currentContext != m_Handle)
    {
        glfwMakeContextCurrent(currentContext);
    }
}

void GlfwWindow::SetPosition(const glm::ivec2 position)
{
    glfwSetWindowPos(m_Handle, position.x, position.y);
}

glm::ivec2 GlfwWindow::GetPosition()
{
    throw std::runtime_error("GlfwWindow::GetPosition is not implemented");
}

void GlfwWindow::Bind()
{
    glfwMakeContextCurrent(m_Handle);
}

void GlfwWindow::Unbind()
{
    glfwMakeContextCurrent(nullptr);
}

glm::ivec2 GlfwWindow::GetDimensions()
{
    int width, height;

    glfwGetWindowSize(m_Handle, &width, &height);

    return {width, height};
}

GlfwWindow::~GlfwWindow()
{
    if (m_Handle != nullptr)
    {
        glfwDestroyWindow(m_Handle);
    }
}
