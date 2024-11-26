#ifndef RENDERTARGET_H
#define RENDERTARGET_H
#include <glm/glm.hpp>

class RenderTarget {
public:
    virtual void Bind() = 0;
    virtual void Unbind() = 0;

    virtual glm::ivec2 GetDimensions() = 0;

    virtual ~RenderTarget() = default;
};

#endif //RENDERTARGET_H
