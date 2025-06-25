//
// Created by alex on 24.11.24.
//

#ifndef SDLWINDOW_H
#define SDLWINDOW_H
#include <cmath>
#include <memory>
#include <string>
#include <unordered_map>

#include "SdlEventQueue.h"
#include "Window.h"
#include <SDL3/SDL.h>


class SdlWindow final : public Window
{
public:
    SdlWindow(int width, int height, std::string title, const std::shared_ptr<SdlEventQueue>& eventQueue);

    SdlWindow(SdlWindow &&other) noexcept;

    SdlWindow &operator=(SdlWindow &&other) noexcept;

    SdlWindow(const SdlWindow &other) = delete;

    SdlWindow &operator=(const SdlWindow &other) = delete;

    bool ShouldClose() override;

    void SwapBuffers() override;

    std::float_t GetFrameTime() override;

    void PollEvents() override;

    void SetPosition(glm::ivec2 position) override;

    glm::ivec2 GetPosition() override;

    bool IsKeyDown(KeyCode key) override;

    bool IsKeyUp(KeyCode key) override;

    bool IsKeyJustPressed(KeyCode key) override;

    bool IsKeyJustReleased(KeyCode key) override;

    void Bind() override;

    void Unbind() override;

    glm::ivec2 GetDimensions() override;

    ~SdlWindow() override;

private:
    void HandleEvent(const SDL_Event &event);
    void UpdateKeyStates();
    void UpdateFrameTime();

    static KeyCode SdlKeyCodeToEnumKeyCode(SDL_Keycode sdlKeyCode);
    static const char* SdlKeyCodeToString(SDL_Keycode sdlKeyCode);

    SDL_GLContext m_Context;
    SDL_Window *m_Handle;
    unsigned int m_WindowId;

    std::string m_Title;
    bool m_ShouldClose = false;
    std::unordered_map<KeyCode, PressedKeyState> m_KeyStates;
    std::shared_ptr<SdlEventQueue> m_EventQueue;

    std::uint64_t m_LastTimerValue = 0.F; // used for retrieving frame time
    std::float_t m_FrameTime = .016F;
};


#endif //SDLWINDOW_H
