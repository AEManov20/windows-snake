#include "SdlEventQueue.h"
#include <iostream>

std::optional<SDL_WindowID> GetWindowIdFromEvent(SDL_Event event)
{
    Uint32 type = event.type;

    if (type >= SDL_EVENT_USER && type <= SDL_EVENT_LAST) {
        return event.user.windowID;
    }
    if (type >= SDL_EVENT_DISPLAY_FIRST && type <= SDL_EVENT_DISPLAY_LAST) {
        return {};
    }
    if (type >= SDL_EVENT_WINDOW_FIRST && type <= SDL_EVENT_WINDOW_LAST) {
        return event.window.windowID;
    }
    switch (type) {
    default:
        return {};

    case SDL_EVENT_RENDER_TARGETS_RESET:
    case SDL_EVENT_RENDER_DEVICE_RESET:
    case SDL_EVENT_RENDER_DEVICE_LOST:
        return event.render.windowID;

    case SDL_EVENT_KEY_DOWN:
    case SDL_EVENT_KEY_UP:
        return event.key.windowID;

    case SDL_EVENT_TEXT_EDITING:
        return event.edit.windowID;

    case SDL_EVENT_TEXT_INPUT:
        return event.text.windowID;

    case SDL_EVENT_TEXT_EDITING_CANDIDATES:
        return event.edit_candidates.windowID;

    case SDL_EVENT_MOUSE_MOTION:
        return event.motion.windowID;

    case SDL_EVENT_MOUSE_BUTTON_DOWN:
    case SDL_EVENT_MOUSE_BUTTON_UP:
        return event.button.windowID;

    case SDL_EVENT_MOUSE_WHEEL:
        return event.wheel.windowID;

    case SDL_EVENT_FINGER_DOWN:
    case SDL_EVENT_FINGER_UP:
    case SDL_EVENT_FINGER_CANCELED:
    case SDL_EVENT_FINGER_MOTION:
        return event.tfinger.windowID;

    case SDL_EVENT_DROP_FILE:
    case SDL_EVENT_DROP_TEXT:
    case SDL_EVENT_DROP_BEGIN:
    case SDL_EVENT_DROP_COMPLETE:
    case SDL_EVENT_DROP_POSITION:
        return event.drop.windowID;

    case SDL_EVENT_PEN_PROXIMITY_IN:
    case SDL_EVENT_PEN_PROXIMITY_OUT:
        return event.pproximity.windowID;

    case SDL_EVENT_PEN_DOWN:
    case SDL_EVENT_PEN_UP:
        return event.ptouch.windowID;

    case SDL_EVENT_PEN_BUTTON_DOWN:
    case SDL_EVENT_PEN_BUTTON_UP:
        return event.pbutton.windowID;

    case SDL_EVENT_PEN_MOTION:
        return event.pmotion.windowID;

    case SDL_EVENT_PEN_AXIS:
        return event.paxis.windowID;
    }
}

void SdlEventQueue::Poll()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        auto windowId = GetWindowIdFromEvent(event);

        // check if event is targeted towards a certain window
        if (windowId.has_value())
        {
            // if it's registered - push
            if (m_EventQueues.contains(*windowId))
            {
                m_EventQueues.at(*windowId).push(event);
            }
            // otherwise inform
            else
            {
                std::cout << "Attempted to push event for non-existent window (" << *windowId << ")\n";
            }

            continue;
        }

        // not targeted towards certain window, broadcast to all
        for (auto& [_, queue] : m_EventQueues)
        {
            queue.push(event);
        }
    }
}

std::optional<SDL_Event> SdlEventQueue::PopForWindow(SDL_WindowID windowId)
{
    if (!m_EventQueues.contains(windowId))
    {
        return {};
    }

    if (m_EventQueues.at(windowId).empty())
    {
        return {};
    }

    auto event = m_EventQueues.at(windowId).front();
    m_EventQueues.at(windowId).pop();

    return event;
}

void SdlEventQueue::RegisterWindow(SDL_WindowID windowId)
{
    if (m_EventQueues.contains(windowId))
    {
        std::cout << "Attempted to register already registered window (" << windowId << ")\n";
        return;
    }

    m_EventQueues.emplace(windowId, std::queue<SDL_Event>());
    std::cout << "Created event queue for window id " << windowId << '\n';
}

void SdlEventQueue::UnregisterWindow(SDL_WindowID windowId)
{
    if (!m_EventQueues.contains(windowId))
    {
        std::cout << "Attempted to unregister non-existent window (" << windowId << ")\n";
        return;
    }

    m_EventQueues.emplace(windowId, std::queue<SDL_Event>());
    std::cout << "Destroyed event queue for window id " << windowId << '\n';
}
