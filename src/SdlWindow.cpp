//
// Created by alex on 24.11.24.
//

#include "SdlWindow.h"

#include <SDL3/SDL_events.h>
#include <iostream>
#include <ostream>
#include <utility>

std::unordered_map<unsigned int, std::queue<SDL_Event> > SdlWindow::s_Events;

SdlWindow::SdlWindow(const int width, const int height, const std::string &title)
{
    m_Title = title;
    m_Handle = SDL_CreateWindow(m_Title.c_str(), width, height, SDL_WINDOW_OPENGL);
    if (!m_Handle)
    {
        throw std::runtime_error("Failed to create window \"" + std::string(SDL_GetError()) + "\"");
    }

    m_Context = SDL_GL_CreateContext(m_Handle);
    if (!m_Context)
    {
        throw std::runtime_error("Failed to create GL context \"" + std::string(SDL_GetError()) + "\"");
    }

    m_WindowId = SDL_GetWindowID(m_Handle);
    if (!m_WindowId)
    {
        throw std::runtime_error("Failed to get window ID \"" + std::string(SDL_GetError()) + "\"");
    }

    s_Events.emplace(m_WindowId, std::queue<SDL_Event>());
    std::cout << "Created event queue for window id " << m_WindowId << std::endl;
}

SdlWindow::SdlWindow(SdlWindow &&other) noexcept : m_Context(std::exchange(other.m_Context, nullptr)),
                                                   m_Handle(std::exchange(other.m_Handle, nullptr)),
                                                   m_WindowId(std::exchange(other.m_WindowId, 0)),
                                                   m_Title(std::move(other.m_Title)),
                                                   m_ShouldClose(std::exchange(other.m_ShouldClose, true))
{
}

SdlWindow &SdlWindow::operator=(SdlWindow &&other) noexcept
{
    SdlWindow tmp(std::move(other));
    std::swap(m_Context, tmp.m_Context);
    std::swap(m_Handle, tmp.m_Handle);
    std::swap(m_WindowId, tmp.m_WindowId);
    std::swap(m_Title, tmp.m_Title);
    std::swap(m_ShouldClose, tmp.m_ShouldClose);

    return *this;
}

bool SdlWindow::ShouldClose()
{
    return m_ShouldClose;
}

void SdlWindow::SwapBuffers()
{
    if (!SDL_GL_SwapWindow(m_Handle))
    {
        std::cout << "WARNING: Could not swap buffers \"" << SDL_GetError() << '"' << std::endl;
    }
}

void SdlWindow::PollEvents()
{
    // std::cout << m_WindowId << "!" << std::endl;
    if (const auto &eventQueue = s_Events.at(m_WindowId); eventQueue.empty())
    {
        SDL_Event event;

        SDL_PumpEvents();
        while (SDL_PeepEvents(&event, 1, SDL_GETEVENT, SDL_EVENT_WINDOW_FIRST, SDL_EVENT_WINDOW_LAST))
        {
            if (event.window.windowID == m_WindowId)
            {
                HandleEvent(event);
            } else
            {
                PushEvent(event.window.windowID, event);
            }
        }
    } else
    {
        for (auto event = PopEvent(); event.has_value(); event = PopEvent())
        {
            HandleEvent(event.value());
        }
    }
}

void SdlWindow::SetPosition(glm::ivec2 position)
{
    if (!SDL_SetWindowPosition(m_Handle, position.x, position.y))
    {
        std::cout << "WARNING: Could not set window position \"" << SDL_GetError() << '"' << std::endl;
    } else
    {
        SDL_SyncWindow(m_Handle);
    }
}

glm::ivec2 SdlWindow::GetPosition()
{
    glm::ivec2 position;
    if (!SDL_GetWindowPosition(m_Handle, &position.x, &position.y))
    {
        std::cout << "WARNING: Could not get window position \"" << SDL_GetError() << std::endl;
        return {0, 0};
    }

    return position;
}

void SdlWindow::Bind()
{
    if (!SDL_GL_MakeCurrent(m_Handle, m_Context))
    {
        std::cout << "WARNING: Could not make GL context \"" << SDL_GetError() << std::endl;
    }
}

void SdlWindow::Unbind()
{
}

glm::ivec2 SdlWindow::GetDimensions()
{
    int width, height;
    if (!SDL_GetWindowSize(m_Handle, &width, &height))
    {
        std::cout << "WARNING: Could not get window size \"" << SDL_GetError() << '"' << std::endl;
        return {0, 0};
    }

    return {width, height};
}

SdlWindow::~SdlWindow()
{
    if (m_Handle != nullptr)
    {
        if (!SDL_GL_DestroyContext(m_Context))
        {
            std::cout << "WARNING: Could not destroy GL context \"" << SDL_GetError() << std::endl;
        }

        SDL_DestroyWindow(m_Handle);

        s_Events.erase(m_WindowId);
        std::cout << "Destroyed queue for window id " << m_WindowId << std::endl;

        m_WindowId = 0;
    }
}

void SdlWindow::PushEvent(const unsigned int windowId, const SDL_Event &event)
{
    if (!s_Events.contains(windowId))
    {
        std::cout << "WARNING: Tried pushing event for a non-existent window id (";
        std::cout << windowId << ")" << std::endl;

        return;
    }

    s_Events.at(windowId).push(event);
}

std::optional<SDL_Event> SdlWindow::PopEvent() const
{
    if (!s_Events.contains(m_WindowId))
    {
        throw std::runtime_error(
            "Tried popping an event for a non-existent window id (" + std::to_string(m_WindowId) + ")");
    }

    if (s_Events.at(m_WindowId).empty())
    {
        return std::nullopt;
    }

    const auto val = s_Events[m_WindowId].front();
    s_Events.at(m_WindowId).pop();

    return val;
}

void SdlWindow::HandleEvent(const SDL_Event &event)
{
    if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED)
    {
        std::cout << m_WindowId << " will close next frame" << std::endl;
        m_ShouldClose = true;
    }
}
