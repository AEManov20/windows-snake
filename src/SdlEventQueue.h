#ifndef SDLEVENTQUEUE_H
#define SDLEVENTQUEUE_H

#include <queue>
#include <unordered_map>
#include <optional>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_video.h>

class SdlEventQueue
{
public:
    void Poll();

    std::optional<SDL_Event> PopForWindow(SDL_WindowID windowId);

private:
    void RegisterWindow(SDL_WindowID windowId);
    void UnregisterWindow(SDL_WindowID windowId);

    std::unordered_map<SDL_WindowID, std::queue<SDL_Event>> m_EventQueues;

    friend class SdlWindow;
};

#endif // SDLEVENTQUEUE_H
