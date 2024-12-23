//
// Created by alex on 12/23/24.
//

#include "GlVertexArray.h"

GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
{
    switch (type)
    {
        case ShaderDataType::Float:
        case ShaderDataType::Float2:
        case ShaderDataType::Float3:
        case ShaderDataType::Float4:
        case ShaderDataType::Mat3:
        case ShaderDataType::Mat4:
            return GL_FLOAT;
        case ShaderDataType::Int:
        case ShaderDataType::Int2:
        case ShaderDataType::Int3:
        case ShaderDataType::Int4:
            return GL_INT;
        case ShaderDataType::UInt:
        case ShaderDataType::UInt2:
        case ShaderDataType::UInt3:
        case ShaderDataType::UInt4:
            return GL_UNSIGNED_INT;
        case ShaderDataType::Bool:
            return GL_BOOL;
    }

    return 0;
}

GlVertexArray::GlVertexArray()
    : m_ArrayId(0), m_VertexBufferIndex(0)
{
    glGenVertexArrays(1, &m_ArrayId);
    if (m_ArrayId == 0)
    {
        throw std::runtime_error("Failed to create vertex array");
    }
}

GlVertexArray::GlVertexArray(GlVertexArray &&other) noexcept
    : m_ArrayId(std::exchange(other.m_ArrayId, 0)),
      m_VertexBufferIndex(std::exchange(other.m_VertexBufferIndex, 0)),
      m_VertexBuffers(std::move(other.m_VertexBuffers)),
      m_IndexBuffer(std::move(other.m_IndexBuffer))
{
}

GlVertexArray &GlVertexArray::operator=(GlVertexArray &&other) noexcept
{
    GlVertexArray tmp(std::move(other));

    std::swap(m_ArrayId, tmp.m_ArrayId);
    std::swap(m_VertexBufferIndex, tmp.m_VertexBufferIndex);
    std::swap(m_VertexBuffers, tmp.m_VertexBuffers);
    std::swap(m_IndexBuffer, tmp.m_IndexBuffer);

    return *this;
}

GlVertexArray::~GlVertexArray()
{
    if (m_ArrayId != 0)
    {
        glDeleteVertexArrays(1, &m_ArrayId);
    }
}

void GlVertexArray::Bind()
{
    glBindVertexArray(m_ArrayId);
}

void GlVertexArray::Unbind()
{
    glBindVertexArray(0);
}

void GlVertexArray::AddVertexBuffer(const std::shared_ptr<VertexBuffer> &vertexBuffer)
{
    glBindVertexArray(m_ArrayId);
    vertexBuffer->Bind();

    const auto &layout = vertexBuffer->GetItemLayout();
    for (const auto &element: layout.GetElements())
    {
        switch (element.m_Type)
        {
            case ShaderDataType::Float:
            case ShaderDataType::Float2:
            case ShaderDataType::Float3:
            case ShaderDataType::Float4:
            {
                glEnableVertexAttribArray(m_VertexBufferIndex);
                glVertexAttribPointer(m_VertexBufferIndex,
                                      static_cast<GLint>(element.GetComponentCount()),
                                      ShaderDataTypeToOpenGLBaseType(element.m_Type),
                                      element.m_Normalized ? GL_TRUE : GL_FALSE,
                                      static_cast<GLsizei>(layout.GetStride()),
                                      reinterpret_cast<const void *>(element.m_Offset));

                m_VertexBufferIndex++;
                break;
            }
            case ShaderDataType::Int:
            case ShaderDataType::Int2:
            case ShaderDataType::Int3:
            case ShaderDataType::Int4:
            case ShaderDataType::UInt:
            case ShaderDataType::UInt2:
            case ShaderDataType::UInt3:
            case ShaderDataType::UInt4:
            case ShaderDataType::Bool:
            {
                glEnableVertexAttribArray(m_VertexBufferIndex);
                glVertexAttribIPointer(m_VertexBufferIndex,
                                       static_cast<GLint>(element.GetComponentCount()),
                                       ShaderDataTypeToOpenGLBaseType(element.m_Type),
                                       static_cast<GLsizei>(layout.GetStride()),
                                       reinterpret_cast<const void *>(element.m_Offset));

                m_VertexBufferIndex++;
                break;
            }
            case ShaderDataType::Mat3:
            case ShaderDataType::Mat4:
            {
                auto count = element.GetComponentCount();

                for (size_t i = 0; i < count; ++i)
                {
                    glEnableVertexAttribArray(m_VertexBufferIndex);
                    glVertexAttribPointer(m_VertexBufferIndex,
                                          count,
                                          ShaderDataTypeToOpenGLBaseType(element.m_Type),
                                          element.m_Normalized ? GL_TRUE : GL_FALSE,
                                          static_cast<GLsizei>(layout.GetStride()),
                                          reinterpret_cast<const void *>(element.m_Offset + sizeof(float) * count * i));
                    glVertexAttribDivisor(m_VertexBufferIndex, 1);
                    m_VertexBufferIndex++;
                }
                break;
            }
        }
    }

    glBindVertexArray(0);

    m_VertexBuffers.push_back(vertexBuffer);
}

void GlVertexArray::SetIndexBuffer(const std::shared_ptr<IndexBuffer> &indexBuffer)
{
    glBindVertexArray(m_ArrayId);
    indexBuffer->Bind();
    glBindVertexArray(0);
}

const std::vector<std::shared_ptr<VertexBuffer> > &GlVertexArray::GetVertexBuffers() const
{
    return m_VertexBuffers;
}

const std::shared_ptr<IndexBuffer> &GlVertexArray::GetIndexBuffer() const
{
    return m_IndexBuffer;
}
