//
// Created by alex on 12/23/24.
//

#ifndef VERTEXARRAY_H
#define VERTEXARRAY_H
#include <memory>

#include "VertexBuffer.h"

class VertexArray
{
public:
    virtual ~VertexArray() = default;

    virtual void Bind() = 0;

    virtual void Unbind() = 0;

    virtual void AddVertexBuffer(const std::shared_ptr<VertexBuffer> &vertexBuffer) = 0;

    virtual void SetIndexBuffer(const std::shared_ptr<IndexBuffer> &indexBuffer) = 0;

    [[nodiscard]] virtual const std::vector<std::shared_ptr<VertexBuffer> > &GetVertexBuffers() const = 0;

    [[nodiscard]] virtual const std::shared_ptr<IndexBuffer> &GetIndexBuffer() const = 0;
};

#endif //VERTEXARRAY_H
