//
// Created by alex on 24.11.24.
//

#ifndef WINDOW_H
#define WINDOW_H
#include "RenderTarget.h"

class Window : RenderTarget {
public:
    virtual bool ShouldClose() = 0;
    virtual void SwapBuffers() = 0;
    virtual void PollEvents() = 0;

    virtual void SetPosition(glm::ivec2 position) = 0;
    virtual glm::ivec2 GetPosition() = 0;
};

#endif //WINDOW_H
