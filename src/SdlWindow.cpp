//
// Created by alex on 24.11.24.
//

#include "SdlWindow.h"
#include "Keyboard.h"
#include "SdlEventQueue.h"

#include <epoxy/gl.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_video.h>
#include <iostream>
#include <memory>
#include <optional>
#include <ostream>
#include <utility>

using std::string;

SdlWindow::SdlWindow(const int width, const int height, string title, const std::shared_ptr<SdlEventQueue>& eventQueue)
    : m_Title(std::move(title)), m_EventQueue(eventQueue) {
  m_Handle =
      SDL_CreateWindow(m_Title.c_str(), width, height, SDL_WINDOW_OPENGL);
  if (m_Handle == nullptr) {
    throw std::runtime_error("Failed to create window \"" +
                             std::string(SDL_GetError()) + "\"");
  }

  m_Context = SDL_GL_CreateContext(m_Handle);
  if (m_Context == nullptr) {
    throw std::runtime_error("Failed to create GL context \"" +
                             std::string(SDL_GetError()) + "\"");
  }

  m_WindowId = SDL_GetWindowID(m_Handle);
  if (m_WindowId == 0) {
    throw std::runtime_error("Failed to get window ID \"" +
                             std::string(SDL_GetError()) + "\"");
  }

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  m_EventQueue->RegisterWindow(m_WindowId);
}

SdlWindow::SdlWindow(SdlWindow &&other) noexcept : m_Context(std::exchange(other.m_Context, nullptr)),
                                                   m_Handle(std::exchange(other.m_Handle, nullptr)),
                                                   m_WindowId(std::exchange(other.m_WindowId, 0)),
                                                   m_Title(std::move(other.m_Title)),
                                                   m_ShouldClose(std::exchange(other.m_ShouldClose, true)),
                                                   m_EventQueue(std::exchange(other.m_EventQueue, nullptr)),
                                                   m_KeyStates(std::move(other.m_KeyStates))
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
    std::swap(m_EventQueue, tmp.m_EventQueue);
    std::swap(m_KeyStates, tmp.m_KeyStates);

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
        std::cout << "WARNING: Could not swap buffers \"" << SDL_GetError() << '"' << '\n';
    }
}

void SdlWindow::PollEvents()
{
    for (auto event = m_EventQueue->PopForWindow(m_WindowId); event.has_value(); event = m_EventQueue->PopForWindow(m_WindowId))
    {
        HandleEvent(*event);
    }
}

void SdlWindow::SetPosition(glm::ivec2 position)
{
    if (!SDL_SetWindowPosition(m_Handle, position.x, position.y))
    {
        std::cout << "WARNING: Could not set window position \"" << SDL_GetError() << '"' << '\n';
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
        std::cout << "WARNING: Could not get window position \"" << SDL_GetError() << '\n';
        return {0, 0};
    }

    return position;
}

bool SdlWindow::IsKeyDown(KeyCode key)
{
    if (!m_KeyStates.contains(key))
    {
        return false;
    }

    return m_KeyStates.at(key) != PressedKeyState::JustReleased;
}

bool SdlWindow::IsKeyUp(KeyCode key)
{
    if (!m_KeyStates.contains(key))
    {
        return true;
    }

    return m_KeyStates.at(key) == PressedKeyState::JustReleased;
}

bool SdlWindow::IsKeyJustPressed(KeyCode key)
{
    if (!m_KeyStates.contains(key))
    {
        return false;
    }

    return m_KeyStates.at(key) == PressedKeyState::JustPressed;
}

bool SdlWindow::IsKeyJustReleased(KeyCode key)
{
    if (!m_KeyStates.contains(key))
    {
        return false;
    }

    return m_KeyStates.at(key) == PressedKeyState::JustReleased;
}

void SdlWindow::Bind()
{
    if (!SDL_GL_MakeCurrent(m_Handle, m_Context))
    {
        std::cout << "WARNING: Could not make GL context \"" << SDL_GetError() << '\n';
    }
}

void SdlWindow::Unbind()
{
}

glm::ivec2 SdlWindow::GetDimensions()
{
    int width;
    int height;
    if (!SDL_GetWindowSize(m_Handle, &width, &height))
    {
        std::cout << "WARNING: Could not get window size \"" << SDL_GetError() << '"' << '\n';
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
            std::cout << "WARNING: Could not destroy GL context \"" << SDL_GetError() << '\n';
        }

        SDL_DestroyWindow(m_Handle);

        m_EventQueue->UnregisterWindow(m_WindowId);
    }
}

void SdlWindow::HandleEvent(const SDL_Event &event)
{
    switch (event.type)
    {
        case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
        {
            std::cout << m_WindowId << " will close next frame" << '\n';
            m_ShouldClose = true;
        }
        case SDL_EVENT_KEY_DOWN:
        {
            if (!event.key.repeat)
            {
                std::cout << "down " << SdlKeyCodeToString(event.key.key) << '\n';
            }
        }
        case SDL_EVENT_KEY_UP:
        {
            if (!event.key.repeat)
            {
                std::cout << "up " << SdlKeyCodeToString(event.key.key) << '\n';
            }
        }
    }
}

const char* SdlWindow::SdlKeyCodeToString(SDL_Keycode sdlKeyCode)
{
    switch (sdlKeyCode) {
        case SDLK_UNKNOWN: return "SDLK_UNKNOWN";
        case SDLK_RETURN: return "SDLK_RETURN";
        case SDLK_ESCAPE: return "SDLK_ESCAPE";
        case SDLK_BACKSPACE: return "SDLK_BACKSPACE";
        case SDLK_TAB: return "SDLK_TAB";
        case SDLK_SPACE: return "SDLK_SPACE";
        case SDLK_EXCLAIM: return "SDLK_EXCLAIM";
        case SDLK_DBLAPOSTROPHE: return "SDLK_DBLAPOSTROPHE";
        case SDLK_HASH: return "SDLK_HASH";
        case SDLK_DOLLAR: return "SDLK_DOLLAR";
        case SDLK_PERCENT: return "SDLK_PERCENT";
        case SDLK_AMPERSAND: return "SDLK_AMPERSAND";
        case SDLK_APOSTROPHE: return "SDLK_APOSTROPHE";
        case SDLK_LEFTPAREN: return "SDLK_LEFTPAREN";
        case SDLK_RIGHTPAREN: return "SDLK_RIGHTPAREN";
        case SDLK_ASTERISK: return "SDLK_ASTERISK";
        case SDLK_PLUS: return "SDLK_PLUS";
        case SDLK_COMMA: return "SDLK_COMMA";
        case SDLK_MINUS: return "SDLK_MINUS";
        case SDLK_PERIOD: return "SDLK_PERIOD";
        case SDLK_SLASH: return "SDLK_SLASH";
        case SDLK_0: return "SDLK_0";
        case SDLK_1: return "SDLK_1";
        case SDLK_2: return "SDLK_2";
        case SDLK_3: return "SDLK_3";
        case SDLK_4: return "SDLK_4";
        case SDLK_5: return "SDLK_5";
        case SDLK_6: return "SDLK_6";
        case SDLK_7: return "SDLK_7";
        case SDLK_8: return "SDLK_8";
        case SDLK_9: return "SDLK_9";
        case SDLK_COLON: return "SDLK_COLON";
        case SDLK_SEMICOLON: return "SDLK_SEMICOLON";
        case SDLK_LESS: return "SDLK_LESS";
        case SDLK_EQUALS: return "SDLK_EQUALS";
        case SDLK_GREATER: return "SDLK_GREATER";
        case SDLK_QUESTION: return "SDLK_QUESTION";
        case SDLK_AT: return "SDLK_AT";
        case SDLK_LEFTBRACKET: return "SDLK_LEFTBRACKET";
        case SDLK_BACKSLASH: return "SDLK_BACKSLASH";
        case SDLK_RIGHTBRACKET: return "SDLK_RIGHTBRACKET";
        case SDLK_CARET: return "SDLK_CARET";
        case SDLK_UNDERSCORE: return "SDLK_UNDERSCORE";
        case SDLK_GRAVE: return "SDLK_GRAVE";
        case SDLK_A: return "SDLK_A";
        case SDLK_B: return "SDLK_B";
        case SDLK_C: return "SDLK_C";
        case SDLK_D: return "SDLK_D";
        case SDLK_E: return "SDLK_E";
        case SDLK_F: return "SDLK_F";
        case SDLK_G: return "SDLK_G";
        case SDLK_H: return "SDLK_H";
        case SDLK_I: return "SDLK_I";
        case SDLK_J: return "SDLK_J";
        case SDLK_K: return "SDLK_K";
        case SDLK_L: return "SDLK_L";
        case SDLK_M: return "SDLK_M";
        case SDLK_N: return "SDLK_N";
        case SDLK_O: return "SDLK_O";
        case SDLK_P: return "SDLK_P";
        case SDLK_Q: return "SDLK_Q";
        case SDLK_R: return "SDLK_R";
        case SDLK_S: return "SDLK_S";
        case SDLK_T: return "SDLK_T";
        case SDLK_U: return "SDLK_U";
        case SDLK_V: return "SDLK_V";
        case SDLK_W: return "SDLK_W";
        case SDLK_X: return "SDLK_X";
        case SDLK_Y: return "SDLK_Y";
        case SDLK_Z: return "SDLK_Z";
        case SDLK_LEFTBRACE: return "SDLK_LEFTBRACE";
        case SDLK_PIPE: return "SDLK_PIPE";
        case SDLK_RIGHTBRACE: return "SDLK_RIGHTBRACE";
        case SDLK_TILDE: return "SDLK_TILDE";
        case SDLK_DELETE: return "SDLK_DELETE";
        case SDLK_PLUSMINUS: return "SDLK_PLUSMINUS";
        case SDLK_CAPSLOCK: return "SDLK_CAPSLOCK";
        case SDLK_F1: return "SDLK_F1";
        case SDLK_F2: return "SDLK_F2";
        case SDLK_F3: return "SDLK_F3";
        case SDLK_F4: return "SDLK_F4";
        case SDLK_F5: return "SDLK_F5";
        case SDLK_F6: return "SDLK_F6";
        case SDLK_F7: return "SDLK_F7";
        case SDLK_F8: return "SDLK_F8";
        case SDLK_F9: return "SDLK_F9";
        case SDLK_F10: return "SDLK_F10";
        case SDLK_F11: return "SDLK_F11";
        case SDLK_F12: return "SDLK_F12";
        case SDLK_PRINTSCREEN: return "SDLK_PRINTSCREEN";
        case SDLK_SCROLLLOCK: return "SDLK_SCROLLLOCK";
        case SDLK_PAUSE: return "SDLK_PAUSE";
        case SDLK_INSERT: return "SDLK_INSERT";
        case SDLK_HOME: return "SDLK_HOME";
        case SDLK_PAGEUP: return "SDLK_PAGEUP";
        case SDLK_END: return "SDLK_END";
        case SDLK_PAGEDOWN: return "SDLK_PAGEDOWN";
        case SDLK_RIGHT: return "SDLK_RIGHT";
        case SDLK_LEFT: return "SDLK_LEFT";
        case SDLK_DOWN: return "SDLK_DOWN";
        case SDLK_UP: return "SDLK_UP";
        case SDLK_NUMLOCKCLEAR: return "SDLK_NUMLOCKCLEAR";
        case SDLK_KP_DIVIDE: return "SDLK_KP_DIVIDE";
        case SDLK_KP_MULTIPLY: return "SDLK_KP_MULTIPLY";
        case SDLK_KP_MINUS: return "SDLK_KP_MINUS";
        case SDLK_KP_PLUS: return "SDLK_KP_PLUS";
        case SDLK_KP_ENTER: return "SDLK_KP_ENTER";
        case SDLK_KP_1: return "SDLK_KP_1";
        case SDLK_KP_2: return "SDLK_KP_2";
        case SDLK_KP_3: return "SDLK_KP_3";
        case SDLK_KP_4: return "SDLK_KP_4";
        case SDLK_KP_5: return "SDLK_KP_5";
        case SDLK_KP_6: return "SDLK_KP_6";
        case SDLK_KP_7: return "SDLK_KP_7";
        case SDLK_KP_8: return "SDLK_KP_8";
        case SDLK_KP_9: return "SDLK_KP_9";
        case SDLK_KP_0: return "SDLK_KP_0";
        case SDLK_KP_PERIOD: return "SDLK_KP_PERIOD";
        case SDLK_APPLICATION: return "SDLK_APPLICATION";
        case SDLK_POWER: return "SDLK_POWER";
        case SDLK_KP_EQUALS: return "SDLK_KP_EQUALS";
        case SDLK_F13: return "SDLK_F13";
        case SDLK_F14: return "SDLK_F14";
        case SDLK_F15: return "SDLK_F15";
        case SDLK_F16: return "SDLK_F16";
        case SDLK_F17: return "SDLK_F17";
        case SDLK_F18: return "SDLK_F18";
        case SDLK_F19: return "SDLK_F19";
        case SDLK_F20: return "SDLK_F20";
        case SDLK_F21: return "SDLK_F21";
        case SDLK_F22: return "SDLK_F22";
        case SDLK_F23: return "SDLK_F23";
        case SDLK_F24: return "SDLK_F24";
        case SDLK_EXECUTE: return "SDLK_EXECUTE";
        case SDLK_HELP: return "SDLK_HELP";
        case SDLK_MENU: return "SDLK_MENU";
        case SDLK_SELECT: return "SDLK_SELECT";
        case SDLK_STOP: return "SDLK_STOP";
        case SDLK_AGAIN: return "SDLK_AGAIN";
        case SDLK_UNDO: return "SDLK_UNDO";
        case SDLK_CUT: return "SDLK_CUT";
        case SDLK_COPY: return "SDLK_COPY";
        case SDLK_PASTE: return "SDLK_PASTE";
        case SDLK_FIND: return "SDLK_FIND";
        case SDLK_MUTE: return "SDLK_MUTE";
        case SDLK_VOLUMEUP: return "SDLK_VOLUMEUP";
        case SDLK_VOLUMEDOWN: return "SDLK_VOLUMEDOWN";
        case SDLK_KP_COMMA: return "SDLK_KP_COMMA";
        case SDLK_KP_EQUALSAS400: return "SDLK_KP_EQUALSAS400";
        case SDLK_ALTERASE: return "SDLK_ALTERASE";
        case SDLK_SYSREQ: return "SDLK_SYSREQ";
        case SDLK_CANCEL: return "SDLK_CANCEL";
        case SDLK_CLEAR: return "SDLK_CLEAR";
        case SDLK_PRIOR: return "SDLK_PRIOR";
        case SDLK_RETURN2: return "SDLK_RETURN2";
        case SDLK_SEPARATOR: return "SDLK_SEPARATOR";
        case SDLK_OUT: return "SDLK_OUT";
        case SDLK_OPER: return "SDLK_OPER";
        case SDLK_CLEARAGAIN: return "SDLK_CLEARAGAIN";
        case SDLK_CRSEL: return "SDLK_CRSEL";
        case SDLK_EXSEL: return "SDLK_EXSEL";
        case SDLK_KP_00: return "SDLK_KP_00";
        case SDLK_KP_000: return "SDLK_KP_000";
        case SDLK_THOUSANDSSEPARATOR: return "SDLK_THOUSANDSSEPARATOR";
        case SDLK_DECIMALSEPARATOR: return "SDLK_DECIMALSEPARATOR";
        case SDLK_CURRENCYUNIT: return "SDLK_CURRENCYUNIT";
        case SDLK_CURRENCYSUBUNIT: return "SDLK_CURRENCYSUBUNIT";
        case SDLK_KP_LEFTPAREN: return "SDLK_KP_LEFTPAREN";
        case SDLK_KP_RIGHTPAREN: return "SDLK_KP_RIGHTPAREN";
        case SDLK_KP_LEFTBRACE: return "SDLK_KP_LEFTBRACE";
        case SDLK_KP_RIGHTBRACE: return "SDLK_KP_RIGHTBRACE";
        case SDLK_KP_TAB: return "SDLK_KP_TAB";
        case SDLK_KP_BACKSPACE: return "SDLK_KP_BACKSPACE";
        case SDLK_KP_A: return "SDLK_KP_A";
        case SDLK_KP_B: return "SDLK_KP_B";
        case SDLK_KP_C: return "SDLK_KP_C";
        case SDLK_KP_D: return "SDLK_KP_D";
        case SDLK_KP_E: return "SDLK_KP_E";
        case SDLK_KP_F: return "SDLK_KP_F";
        case SDLK_KP_XOR: return "SDLK_KP_XOR";
        case SDLK_KP_POWER: return "SDLK_KP_POWER";
        case SDLK_KP_PERCENT: return "SDLK_KP_PERCENT";
        case SDLK_KP_LESS: return "SDLK_KP_LESS";
        case SDLK_KP_GREATER: return "SDLK_KP_GREATER";
        case SDLK_KP_AMPERSAND: return "SDLK_KP_AMPERSAND";
        case SDLK_KP_DBLAMPERSAND: return "SDLK_KP_DBLAMPERSAND";
        case SDLK_KP_VERTICALBAR: return "SDLK_KP_VERTICALBAR";
        case SDLK_KP_DBLVERTICALBAR: return "SDLK_KP_DBLVERTICALBAR";
        case SDLK_KP_COLON: return "SDLK_KP_COLON";
        case SDLK_KP_HASH: return "SDLK_KP_HASH";
        case SDLK_KP_SPACE: return "SDLK_KP_SPACE";
        case SDLK_KP_AT: return "SDLK_KP_AT";
        case SDLK_KP_EXCLAM: return "SDLK_KP_EXCLAM";
        case SDLK_KP_MEMSTORE: return "SDLK_KP_MEMSTORE";
        case SDLK_KP_MEMRECALL: return "SDLK_KP_MEMRECALL";
        case SDLK_KP_MEMCLEAR: return "SDLK_KP_MEMCLEAR";
        case SDLK_KP_MEMADD: return "SDLK_KP_MEMADD";
        case SDLK_KP_MEMSUBTRACT: return "SDLK_KP_MEMSUBTRACT";
        case SDLK_KP_MEMMULTIPLY: return "SDLK_KP_MEMMULTIPLY";
        case SDLK_KP_MEMDIVIDE: return "SDLK_KP_MEMDIVIDE";
        case SDLK_KP_PLUSMINUS: return "SDLK_KP_PLUSMINUS";
        case SDLK_KP_CLEAR: return "SDLK_KP_CLEAR";
        case SDLK_KP_CLEARENTRY: return "SDLK_KP_CLEARENTRY";
        case SDLK_KP_BINARY: return "SDLK_KP_BINARY";
        case SDLK_KP_OCTAL: return "SDLK_KP_OCTAL";
        case SDLK_KP_DECIMAL: return "SDLK_KP_DECIMAL";
        case SDLK_KP_HEXADECIMAL: return "SDLK_KP_HEXADECIMAL";
        case SDLK_LCTRL: return "SDLK_LCTRL";
        case SDLK_LSHIFT: return "SDLK_LSHIFT";
        case SDLK_LALT: return "SDLK_LALT";
        case SDLK_LGUI: return "SDLK_LGUI";
        case SDLK_RCTRL: return "SDLK_RCTRL";
        case SDLK_RSHIFT: return "SDLK_RSHIFT";
        case SDLK_RALT: return "SDLK_RALT";
        case SDLK_RGUI: return "SDLK_RGUI";
        case SDLK_MODE: return "SDLK_MODE";
        case SDLK_SLEEP: return "SDLK_SLEEP";
        case SDLK_WAKE: return "SDLK_WAKE";
        case SDLK_CHANNEL_INCREMENT: return "SDLK_CHANNEL_INCREMENT";
        case SDLK_CHANNEL_DECREMENT: return "SDLK_CHANNEL_DECREMENT";
        case SDLK_MEDIA_PLAY: return "SDLK_MEDIA_PLAY";
        case SDLK_MEDIA_PAUSE: return "SDLK_MEDIA_PAUSE";
        case SDLK_MEDIA_RECORD: return "SDLK_MEDIA_RECORD";
        case SDLK_MEDIA_FAST_FORWARD: return "SDLK_MEDIA_FAST_FORWARD";
        case SDLK_MEDIA_REWIND: return "SDLK_MEDIA_REWIND";
        case SDLK_MEDIA_NEXT_TRACK: return "SDLK_MEDIA_NEXT_TRACK";
        case SDLK_MEDIA_PREVIOUS_TRACK: return "SDLK_MEDIA_PREVIOUS_TRACK";
        case SDLK_MEDIA_STOP: return "SDLK_MEDIA_STOP";
        case SDLK_MEDIA_EJECT: return "SDLK_MEDIA_EJECT";
        case SDLK_MEDIA_PLAY_PAUSE: return "SDLK_MEDIA_PLAY_PAUSE";
        case SDLK_MEDIA_SELECT: return "SDLK_MEDIA_SELECT";
        case SDLK_AC_NEW: return "SDLK_AC_NEW";
        case SDLK_AC_OPEN: return "SDLK_AC_OPEN";
        case SDLK_AC_CLOSE: return "SDLK_AC_CLOSE";
        case SDLK_AC_EXIT: return "SDLK_AC_EXIT";
        case SDLK_AC_SAVE: return "SDLK_AC_SAVE";
        case SDLK_AC_PRINT: return "SDLK_AC_PRINT";
        case SDLK_AC_PROPERTIES: return "SDLK_AC_PROPERTIES";
        case SDLK_AC_SEARCH: return "SDLK_AC_SEARCH";
        case SDLK_AC_HOME: return "SDLK_AC_HOME";
        case SDLK_AC_BACK: return "SDLK_AC_BACK";
        case SDLK_AC_FORWARD: return "SDLK_AC_FORWARD";
        case SDLK_AC_STOP: return "SDLK_AC_STOP";
        case SDLK_AC_REFRESH: return "SDLK_AC_REFRESH";
        case SDLK_AC_BOOKMARKS: return "SDLK_AC_BOOKMARKS";
        case SDLK_SOFTLEFT: return "SDLK_SOFTLEFT";
        case SDLK_SOFTRIGHT: return "SDLK_SOFTRIGHT";
        case SDLK_CALL: return "SDLK_CALL";
        case SDLK_ENDCALL: return "SDLK_ENDCALL";
        case SDLK_LEFT_TAB: return "SDLK_LEFT_TAB";
        case SDLK_LEVEL5_SHIFT: return "SDLK_LEVEL5_SHIFT";
        case SDLK_MULTI_KEY_COMPOSE: return "SDLK_MULTI_KEY_COMPOSE";
        case SDLK_LMETA: return "SDLK_LMETA";
        case SDLK_RMETA: return "SDLK_RMETA";
        case SDLK_LHYPER: return "SDLK_LHYPER";
        case SDLK_RHYPER: return "SDLK_RHYPER";
        default: return "NONE";
    }
}

KeyCode SdlWindow::SdlKeyCodeToEnumKeyCode(SDL_Keycode sdlKeyCode)
{
    switch (sdlKeyCode) {
        case SDLK_RETURN: return KeyCode::Return;
        case SDLK_ESCAPE: return KeyCode::Escape;
        case SDLK_BACKSPACE: return KeyCode::Backspace;
        case SDLK_TAB: return KeyCode::Tab;
        case SDLK_SPACE: return KeyCode::Space;
        case SDLK_EXCLAIM: return KeyCode::Exclaim;
        case SDLK_DBLAPOSTROPHE: return KeyCode::DoubleApostrophe;
        case SDLK_HASH: return KeyCode::Hash;
        case SDLK_DOLLAR: return KeyCode::Dollar;
        case SDLK_PERCENT: return KeyCode::Percent;
        case SDLK_AMPERSAND: return KeyCode::Ampersand;
        case SDLK_APOSTROPHE: return KeyCode::Apostrophe;
        case SDLK_LEFTPAREN: return KeyCode::LeftParen;
        case SDLK_RIGHTPAREN: return KeyCode::RightParen;
        case SDLK_ASTERISK: return KeyCode::Asterisk;
        case SDLK_PLUS: return KeyCode::Plus;
        case SDLK_COMMA: return KeyCode::Comma;
        case SDLK_MINUS: return KeyCode::Minus;
        case SDLK_PERIOD: return KeyCode::Period;
        case SDLK_SLASH: return KeyCode::Slash;
        case SDLK_0: return KeyCode::Number0;
        case SDLK_1: return KeyCode::Number1;
        case SDLK_2: return KeyCode::Number2;
        case SDLK_3: return KeyCode::Number3;
        case SDLK_4: return KeyCode::Number4;
        case SDLK_5: return KeyCode::Number5;
        case SDLK_6: return KeyCode::Number6;
        case SDLK_7: return KeyCode::Number7;
        case SDLK_8: return KeyCode::Number8;
        case SDLK_9: return KeyCode::Number9;
        case SDLK_COLON: return KeyCode::Colon;
        case SDLK_SEMICOLON: return KeyCode::Semicolon;
        case SDLK_LESS: return KeyCode::Less;
        case SDLK_EQUALS: return KeyCode::Equals;
        case SDLK_GREATER: return KeyCode::Greater;
        case SDLK_QUESTION: return KeyCode::Question;
        case SDLK_AT: return KeyCode::At;
        case SDLK_LEFTBRACKET: return KeyCode::LeftBracket;
        case SDLK_BACKSLASH: return KeyCode::Backslash;
        case SDLK_RIGHTBRACKET: return KeyCode::RightBracket;
        case SDLK_CARET: return KeyCode::Caret;
        case SDLK_UNDERSCORE: return KeyCode::Underscore;
        case SDLK_GRAVE: return KeyCode::Grave;
        case SDLK_A: return KeyCode::A;
        case SDLK_B: return KeyCode::B;
        case SDLK_C: return KeyCode::C;
        case SDLK_D: return KeyCode::D;
        case SDLK_E: return KeyCode::E;
        case SDLK_F: return KeyCode::F;
        case SDLK_G: return KeyCode::G;
        case SDLK_H: return KeyCode::H;
        case SDLK_I: return KeyCode::I;
        case SDLK_J: return KeyCode::J;
        case SDLK_K: return KeyCode::K;
        case SDLK_L: return KeyCode::L;
        case SDLK_M: return KeyCode::M;
        case SDLK_N: return KeyCode::N;
        case SDLK_O: return KeyCode::O;
        case SDLK_P: return KeyCode::P;
        case SDLK_Q: return KeyCode::Q;
        case SDLK_R: return KeyCode::R;
        case SDLK_S: return KeyCode::S;
        case SDLK_T: return KeyCode::T;
        case SDLK_U: return KeyCode::U;
        case SDLK_V: return KeyCode::V;
        case SDLK_W: return KeyCode::W;
        case SDLK_X: return KeyCode::X;
        case SDLK_Y: return KeyCode::Y;
        case SDLK_Z: return KeyCode::Z;
        case SDLK_LEFTBRACE: return KeyCode::LeftBrace;
        case SDLK_PIPE: return KeyCode::Pipe;
        case SDLK_RIGHTBRACE: return KeyCode::RightBrace;
        case SDLK_TILDE: return KeyCode::Tilde;
        case SDLK_DELETE: return KeyCode::Delete;
        case SDLK_PLUSMINUS: return KeyCode::PlusMinus;
        case SDLK_CAPSLOCK: return KeyCode::CapsLock;
        case SDLK_F1: return KeyCode::F1;
        case SDLK_F2: return KeyCode::F2;
        case SDLK_F3: return KeyCode::F3;
        case SDLK_F4: return KeyCode::F4;
        case SDLK_F5: return KeyCode::F5;
        case SDLK_F6: return KeyCode::F6;
        case SDLK_F7: return KeyCode::F7;
        case SDLK_F8: return KeyCode::F8;
        case SDLK_F9: return KeyCode::F9;
        case SDLK_F10: return KeyCode::F10;
        case SDLK_F11: return KeyCode::F11;
        case SDLK_F12: return KeyCode::F12;
        case SDLK_PRINTSCREEN: return KeyCode::PrintScreen;
        case SDLK_SCROLLLOCK: return KeyCode::ScrollLock;
        case SDLK_PAUSE: return KeyCode::Pause;
        case SDLK_INSERT: return KeyCode::Insert;
        case SDLK_HOME: return KeyCode::Home;
        case SDLK_PAGEUP: return KeyCode::PageUp;
        case SDLK_END: return KeyCode::End;
        case SDLK_PAGEDOWN: return KeyCode::PageDown;
        case SDLK_RIGHT: return KeyCode::Right;
        case SDLK_LEFT: return KeyCode::Left;
        case SDLK_DOWN: return KeyCode::Down;
        case SDLK_UP: return KeyCode::Up;
        case SDLK_NUMLOCKCLEAR: return KeyCode::NumLockClear;
        case SDLK_KP_DIVIDE: return KeyCode::KeyPadDivide;
        case SDLK_KP_MULTIPLY: return KeyCode::KeyPadMultiply;
        case SDLK_KP_MINUS: return KeyCode::KeyPadMinus;
        case SDLK_KP_PLUS: return KeyCode::KeyPadPlus;
        case SDLK_KP_ENTER: return KeyCode::KeyPadEnter;
        case SDLK_KP_1: return KeyCode::KeyPad1;
        case SDLK_KP_2: return KeyCode::KeyPad2;
        case SDLK_KP_3: return KeyCode::KeyPad3;
        case SDLK_KP_4: return KeyCode::KeyPad4;
        case SDLK_KP_5: return KeyCode::KeyPad5;
        case SDLK_KP_6: return KeyCode::KeyPad6;
        case SDLK_KP_7: return KeyCode::KeyPad7;
        case SDLK_KP_8: return KeyCode::KeyPad8;
        case SDLK_KP_9: return KeyCode::KeyPad9;
        case SDLK_KP_0: return KeyCode::KeyPad0;
        case SDLK_KP_PERIOD: return KeyCode::KeyPadPeriod;
        case SDLK_APPLICATION: return KeyCode::Application;
        case SDLK_POWER: return KeyCode::Power;
        case SDLK_KP_EQUALS: return KeyCode::KeyPadEquals;
        case SDLK_F13: return KeyCode::F13;
        case SDLK_F14: return KeyCode::F14;
        case SDLK_F15: return KeyCode::F15;
        case SDLK_F16: return KeyCode::F16;
        case SDLK_F17: return KeyCode::F17;
        case SDLK_F18: return KeyCode::F18;
        case SDLK_F19: return KeyCode::F19;
        case SDLK_F20: return KeyCode::F20;
        case SDLK_F21: return KeyCode::F21;
        case SDLK_F22: return KeyCode::F22;
        case SDLK_F23: return KeyCode::F23;
        case SDLK_F24: return KeyCode::F24;
        case SDLK_EXECUTE: return KeyCode::Execute;
        case SDLK_HELP: return KeyCode::Help;
        case SDLK_MENU: return KeyCode::Menu;
        case SDLK_SELECT: return KeyCode::Select;
        case SDLK_STOP: return KeyCode::Stop;
        case SDLK_AGAIN: return KeyCode::Again;
        case SDLK_UNDO: return KeyCode::Undo;
        case SDLK_CUT: return KeyCode::Cut;
        case SDLK_COPY: return KeyCode::Copy;
        case SDLK_PASTE: return KeyCode::Paste;
        case SDLK_FIND: return KeyCode::Find;
        case SDLK_MUTE: return KeyCode::Mute;
        case SDLK_VOLUMEUP: return KeyCode::Volumeup;
        case SDLK_VOLUMEDOWN: return KeyCode::Volumedown;
        case SDLK_KP_COMMA: return KeyCode::KeyPadComma;
        case SDLK_KP_EQUALSAS400: return KeyCode::KeyPadEqualsAs400;
        case SDLK_ALTERASE: return KeyCode::AltErase;
        case SDLK_SYSREQ: return KeyCode::SysReq;
        case SDLK_CANCEL: return KeyCode::Cancel;
        case SDLK_CLEAR: return KeyCode::Clear;
        case SDLK_PRIOR: return KeyCode::Prior;
        case SDLK_RETURN2: return KeyCode::Return2;
        case SDLK_SEPARATOR: return KeyCode::Separator;
        case SDLK_OUT: return KeyCode::Out;
        case SDLK_OPER: return KeyCode::Oper;
        case SDLK_CLEARAGAIN: return KeyCode::ClearAgain;
        case SDLK_CRSEL: return KeyCode::Crsel;
        case SDLK_EXSEL: return KeyCode::Exsel;
        case SDLK_KP_00: return KeyCode::KeyPad00;
        case SDLK_KP_000: return KeyCode::KeyPad000;
        case SDLK_THOUSANDSSEPARATOR: return KeyCode::ThousandsSeparator;
        case SDLK_DECIMALSEPARATOR: return KeyCode::DecimalSeparator;
        case SDLK_CURRENCYUNIT: return KeyCode::CurrencyUnit;
        case SDLK_CURRENCYSUBUNIT: return KeyCode::CurrencySubunit;
        case SDLK_KP_LEFTPAREN: return KeyCode::KeyPadLeftParen;
        case SDLK_KP_RIGHTPAREN: return KeyCode::KeyPadRightParen;
        case SDLK_KP_LEFTBRACE: return KeyCode::KeyPadLeftBrace;
        case SDLK_KP_RIGHTBRACE: return KeyCode::KeyPadRightBrace;
        case SDLK_KP_TAB: return KeyCode::KeyPadTab;
        case SDLK_KP_BACKSPACE: return KeyCode::KeyPadBackspace;
        case SDLK_KP_A: return KeyCode::KeyPadA;
        case SDLK_KP_B: return KeyCode::KeyPadB;
        case SDLK_KP_C: return KeyCode::KeyPadC;
        case SDLK_KP_D: return KeyCode::KeyPadD;
        case SDLK_KP_E: return KeyCode::KeyPadE;
        case SDLK_KP_F: return KeyCode::KeyPadF;
        case SDLK_KP_XOR: return KeyCode::KeyPadXor;
        case SDLK_KP_POWER: return KeyCode::KeyPadPower;
        case SDLK_KP_PERCENT: return KeyCode::KeyPadPercent;
        case SDLK_KP_LESS: return KeyCode::KeyPadLess;
        case SDLK_KP_GREATER: return KeyCode::KeyPadGreater;
        case SDLK_KP_AMPERSAND: return KeyCode::KeyPadAmpersand;
        case SDLK_KP_DBLAMPERSAND: return KeyCode::KeyPadDoubleAmpersand;
        case SDLK_KP_VERTICALBAR: return KeyCode::KeyPadVerticalBar;
        case SDLK_KP_DBLVERTICALBAR: return KeyCode::KeyPadDblverticalBar;
        case SDLK_KP_COLON: return KeyCode::KeyPadColon;
        case SDLK_KP_HASH: return KeyCode::KeyPadHash;
        case SDLK_KP_SPACE: return KeyCode::KeyPadSpace;
        case SDLK_KP_AT: return KeyCode::KeyPadAt;
        case SDLK_KP_EXCLAM: return KeyCode::KeyPadExclam;
        case SDLK_KP_MEMSTORE: return KeyCode::KeyPadMemStore;
        case SDLK_KP_MEMRECALL: return KeyCode::KeyPadMemRecall;
        case SDLK_KP_MEMCLEAR: return KeyCode::KeyPadMemClear;
        case SDLK_KP_MEMADD: return KeyCode::KeyPadMemAdd;
        case SDLK_KP_MEMSUBTRACT: return KeyCode::KeyPadMemSubtract;
        case SDLK_KP_MEMMULTIPLY: return KeyCode::KeyPadMemMultiply;
        case SDLK_KP_MEMDIVIDE: return KeyCode::KeyPadMemDivide;
        case SDLK_KP_PLUSMINUS: return KeyCode::KeyPadPlusMinus;
        case SDLK_KP_CLEAR: return KeyCode::KeyPadClear;
        case SDLK_KP_CLEARENTRY: return KeyCode::KeyPadClearEntry;
        case SDLK_KP_BINARY: return KeyCode::KeyPadBinary;
        case SDLK_KP_OCTAL: return KeyCode::KeyPadOctal;
        case SDLK_KP_DECIMAL: return KeyCode::KeyPadDecimal;
        case SDLK_KP_HEXADECIMAL: return KeyCode::KeyPadHexadecimal;
        case SDLK_LCTRL: return KeyCode::LeftControl;
        case SDLK_LSHIFT: return KeyCode::LeftShift;
        case SDLK_LALT: return KeyCode::LeftAlt;
        case SDLK_LGUI: return KeyCode::LeftGui;
        case SDLK_RCTRL: return KeyCode::RightControl;
        case SDLK_RSHIFT: return KeyCode::RightShift;
        case SDLK_RALT: return KeyCode::RightAlt;
        case SDLK_RGUI: return KeyCode::RightGui;
        case SDLK_MODE: return KeyCode::Mode;
        case SDLK_SLEEP: return KeyCode::Sleep;
        case SDLK_WAKE: return KeyCode::Wake;
        case SDLK_CHANNEL_INCREMENT: return KeyCode::ChannelIncrement;
        case SDLK_CHANNEL_DECREMENT: return KeyCode::ChannelDecrement;
        case SDLK_MEDIA_PLAY: return KeyCode::MediaPlay;
        case SDLK_MEDIA_PAUSE: return KeyCode::MediaPause;
        case SDLK_MEDIA_RECORD: return KeyCode::MediaRecord;
        case SDLK_MEDIA_FAST_FORWARD: return KeyCode::MediaFastForward;
        case SDLK_MEDIA_REWIND: return KeyCode::MediaRewind;
        case SDLK_MEDIA_NEXT_TRACK: return KeyCode::MediaNextTrack;
        case SDLK_MEDIA_PREVIOUS_TRACK: return KeyCode::MediaPreviousTrack;
        case SDLK_MEDIA_STOP: return KeyCode::MediaStop;
        case SDLK_MEDIA_EJECT: return KeyCode::MediaEject;
        case SDLK_MEDIA_PLAY_PAUSE: return KeyCode::MediaPlayPause;
        case SDLK_MEDIA_SELECT: return KeyCode::MediaSelect;
        case SDLK_AC_NEW: return KeyCode::AcNew;
        case SDLK_AC_OPEN: return KeyCode::AcOpen;
        case SDLK_AC_CLOSE: return KeyCode::AcClose;
        case SDLK_AC_EXIT: return KeyCode::AcExit;
        case SDLK_AC_SAVE: return KeyCode::AcSave;
        case SDLK_AC_PRINT: return KeyCode::AcPrint;
        case SDLK_AC_PROPERTIES: return KeyCode::AcProperties;
        case SDLK_AC_SEARCH: return KeyCode::AcSearch;
        case SDLK_AC_HOME: return KeyCode::AcHome;
        case SDLK_AC_BACK: return KeyCode::AcBack;
        case SDLK_AC_FORWARD: return KeyCode::AcForward;
        case SDLK_AC_STOP: return KeyCode::AcStop;
        case SDLK_AC_REFRESH: return KeyCode::AcRefresh;
        case SDLK_AC_BOOKMARKS: return KeyCode::AcBookmarks;
        case SDLK_SOFTLEFT: return KeyCode::SoftLeft;
        case SDLK_SOFTRIGHT: return KeyCode::SoftRight;
        case SDLK_CALL: return KeyCode::Call;
        case SDLK_ENDCALL: return KeyCode::EndCall;
        case SDLK_LEFT_TAB: return KeyCode::LeftTab;
        case SDLK_LEVEL5_SHIFT: return KeyCode::Level5Shift;
        case SDLK_MULTI_KEY_COMPOSE: return KeyCode::MultiKeyCompose;
        case SDLK_LMETA: return KeyCode::LeftMeta;
        case SDLK_RMETA: return KeyCode::RightMeta;
        case SDLK_LHYPER: return KeyCode::LeftHyper;
        case SDLK_RHYPER: return KeyCode::RightHyper;
        default: return KeyCode::Unknown;
    }
}
