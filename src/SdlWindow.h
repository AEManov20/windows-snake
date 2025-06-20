//
// Created by alex on 24.11.24.
//

#ifndef SDLWINDOW_H
#define SDLWINDOW_H
#include <optional>
#include <queue>
#include <string>
#include <unordered_map>

#include "Window.h"
#include <SDL3/SDL.h>


class SdlWindow final : public Window
{
public:
    SdlWindow(int width, int height, std::string title);

    SdlWindow(SdlWindow &&other) noexcept;

    SdlWindow &operator=(SdlWindow &&other) noexcept;

    SdlWindow(const SdlWindow &other) = delete;

    SdlWindow &operator=(const SdlWindow &other) = delete;

    bool ShouldClose() override;

    void SwapBuffers() override;

    void PollEvents() override;

    void SetPosition(glm::ivec2 position) override;

    glm::ivec2 GetPosition() override;

    void Bind() override;

    void Unbind() override;

    glm::ivec2 GetDimensions() override;

    ~SdlWindow() override;

private:
    // pushes event in the events map and relates it to a window id
    static void PushEvent(unsigned int windowId, const SDL_Event &event);

    // pops an event off the event map where the queue is related to the
    // window id of the current instance
    [[nodiscard]] std::optional<SDL_Event> PopEvent() const;

    void HandleEvent(const SDL_Event &event);

    static std::unordered_map<unsigned int, std::queue<SDL_Event> > s_Events;

    SDL_GLContext m_Context;
    SDL_Window *m_Handle;
    unsigned int m_WindowId;

    std::string m_Title;
    bool m_ShouldClose = false;
};


#endif //SDLWINDOW_H
