//
// Created by alex on 12/23/24.
//

#ifndef GLVERTEXARRAY_H
#define GLVERTEXARRAY_H
#include "VertexArray.h"
#include <epoxy/gl.h>


class GlVertexArray : public VertexArray
{
public:
    GlVertexArray();

    GlVertexArray(GlVertexArray &&other) noexcept;

    GlVertexArray &operator=(GlVertexArray &&other) noexcept;

    GlVertexArray(const GlVertexArray &other) = delete;

    GlVertexArray &operator=(const GlVertexArray &) = delete;

    ~GlVertexArray() override;

    void Bind() override;

    void Unbind() override;

    void AddVertexBuffer(const std::shared_ptr<VertexBuffer> &vertexBuffer) override;

    void SetIndexBuffer(const std::shared_ptr<IndexBuffer> &indexBuffer) override;

    [[nodiscard]] const std::vector<std::shared_ptr<VertexBuffer> > &GetVertexBuffers() const override;

    [[nodiscard]] const std::shared_ptr<IndexBuffer> &GetIndexBuffer() const override;

private:
    GLuint m_ArrayId;
    GLuint m_VertexBufferIndex;
    std::vector<std::shared_ptr<VertexBuffer> > m_VertexBuffers;
    std::shared_ptr<IndexBuffer> m_IndexBuffer;
};


#endif //GLVERTEXARRAY_H
