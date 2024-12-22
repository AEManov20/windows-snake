//
// Created by alex on 12/22/24.
//

#ifndef GLBUFFER_H
#define GLBUFFER_H

#include <epoxy/gl.h>
#include "VertexBuffer.h"


class GlVertexBuffer : public VertexBuffer
{
public:
    explicit GlVertexBuffer(size_t size, BufferItemLayout layout);

    explicit GlVertexBuffer(const float* data, size_t count, BufferItemLayout layout);

    GlVertexBuffer(GlVertexBuffer &&other) noexcept;

    GlVertexBuffer &operator=(GlVertexBuffer &&other) noexcept;

    GlVertexBuffer(const GlVertexBuffer &other) = delete;

    GlVertexBuffer &operator=(const GlVertexBuffer &) = delete;

    ~GlVertexBuffer() override;

    void Bind() const override;

    void Unbind() const override;

    void SetData(const void *data, size_t size) override;

    [[nodiscard]] const BufferItemLayout &GetItemLayout() const override;

    void SetItemLayout(const BufferItemLayout &layout) override;

private:
    GLuint m_BufferId;
    BufferItemLayout m_Layout;
    bool m_Immutable;
};

#endif //GLBUFFER_H
