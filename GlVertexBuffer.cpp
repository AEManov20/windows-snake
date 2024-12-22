//
// Created by alex on 12/22/24.
//

#include "GlVertexBuffer.h"

#include <stdexcept>
#include <utility>

GlVertexBuffer::GlVertexBuffer(const size_t size, const BufferItemLayout layout)
    : m_BufferId(0), m_Layout(layout), m_Immutable(false)
{
    glGenBuffers(1, &m_BufferId);
    if (m_BufferId == 0)
    {
        throw std::runtime_error("Failed to create vertex buffer");
    }

    glBindBuffer(GL_ARRAY_BUFFER, m_BufferId);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(size), nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

GlVertexBuffer::GlVertexBuffer(const float *data, size_t count, BufferItemLayout layout)
    : m_BufferId(0), m_Layout(std::move(layout)), m_Immutable(true)
{
    glGenBuffers(1, &m_BufferId);
    if (m_BufferId == 0)
    {
        throw std::runtime_error("Failed to create vertex buffer");
    }

    glBindBuffer(GL_ARRAY_BUFFER, m_BufferId);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(count * sizeof(float)), data, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

GlVertexBuffer::GlVertexBuffer(GlVertexBuffer &&other) noexcept
    : m_BufferId(std::exchange(other.m_BufferId, 0)),
      m_Layout(std::exchange(other.m_Layout, {})),
      m_Immutable(other.m_Immutable)
{
}

GlVertexBuffer &GlVertexBuffer::operator=(GlVertexBuffer &&other) noexcept
{
    GlVertexBuffer tmp(std::move(other));
    std::swap(m_BufferId, tmp.m_BufferId);
    std::swap(m_Layout, tmp.m_Layout);
    std::swap(m_Immutable, tmp.m_Immutable);

    return *this;
}

GlVertexBuffer::~GlVertexBuffer()
{
    if (m_BufferId != 0)
    {
        glDeleteBuffers(1, &m_BufferId);
    }
}

void GlVertexBuffer::Bind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, m_BufferId);
}

void GlVertexBuffer::Unbind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void GlVertexBuffer::SetData(const void *data, size_t size)
{
    if (m_Immutable)
        throw std::runtime_error("Cannot set data on an immutable buffer");

    glBindBuffer(GL_ARRAY_BUFFER, m_BufferId);
    glBufferSubData(GL_ARRAY_BUFFER, 0, static_cast<GLsizeiptr>(size), data);
}

const BufferItemLayout &GlVertexBuffer::GetItemLayout() const
{
    return m_Layout;
}

void GlVertexBuffer::SetItemLayout(const BufferItemLayout &layout)
{
    m_Layout = layout;
}
