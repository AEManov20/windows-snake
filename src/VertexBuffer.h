//
// Created by alex on 12/22/24.
//

#ifndef VERTEXBUFFER_H
#define VERTEXBUFFER_H

#include <cstdint>
#include <string>
#include <utility>
#include <vector>

enum class ShaderDataType : std::uint8_t
{
    Float,
    Float2,
    Float3,
    Float4,
    Int,
    Int2,
    Int3,
    Int4,
    UInt,
    UInt2,
    UInt3,
    UInt4,
    Bool,
    Mat3,
    Mat4,
};

inline uint32_t ShaderDataTypeSize(const ShaderDataType type)
{
    switch (type)
    {
        case ShaderDataType::Float: return 4;
        case ShaderDataType::Float2: return 4 * 2;
        case ShaderDataType::Float3: return 4 * 3;
        case ShaderDataType::Float4: return 4 * 4;
        case ShaderDataType::UInt:
        case ShaderDataType::Int: return 4;
        case ShaderDataType::UInt2:
        case ShaderDataType::Int2: return 4 * 2;
        case ShaderDataType::UInt3:
        case ShaderDataType::Int3: return 4 * 3;
        case ShaderDataType::UInt4:
        case ShaderDataType::Int4: return 4 * 4;
        case ShaderDataType::Bool: return 1;
        case ShaderDataType::Mat4: return 4 * 4 * 4;
        case ShaderDataType::Mat3: return 4 * 3 * 3;
    }

    return 0;
}

struct BufferElement
{
    std::string m_Name;
    ShaderDataType m_Type;
    size_t m_Size;
    size_t m_Offset = 0;
    bool m_Normalized;

    explicit BufferElement(const ShaderDataType type, std::string name, const bool normalized = false)
        : m_Name(std::move(name)), m_Type(type), m_Size(ShaderDataTypeSize(type)), m_Normalized(normalized)
    {
    }

    [[nodiscard]] size_t GetComponentCount() const
    {
        switch (m_Type)
        {
            case ShaderDataType::Float: return 1;
            case ShaderDataType::Float2: return 2;
            case ShaderDataType::Float3: return 3;
            case ShaderDataType::Float4: return 4;
            case ShaderDataType::UInt:
            case ShaderDataType::Int: return 1;
            case ShaderDataType::UInt2:
            case ShaderDataType::Int2: return 2;
            case ShaderDataType::UInt3:
            case ShaderDataType::Int3: return 3;
            case ShaderDataType::UInt4:
            case ShaderDataType::Int4: return 4;
            case ShaderDataType::Bool: return 1;
            case ShaderDataType::Mat4: return 4;
            case ShaderDataType::Mat3: return 3;
        }

        return 0;
    }
};

struct BufferItemLayout
{
    BufferItemLayout(const std::initializer_list<BufferElement> &elements)
        : m_Elements(elements)
    {
        CalculateOffsetsAndStride();
    }

    [[nodiscard]] size_t GetStride() const;

    [[nodiscard]] const std::vector<BufferElement> &GetElements() const { return m_Elements; }

private:
    void CalculateOffsetsAndStride()
    {
        size_t offset = 0;
        m_Stride = 0;
        for (auto &element: m_Elements)
        {
            element.m_Offset = offset;
            offset += element.m_Size;
            m_Stride += element.m_Size;
        }
    }

    std::vector<BufferElement> m_Elements;
    size_t m_Stride = 0;
};

inline size_t BufferItemLayout::GetStride() const { return m_Stride; }

class VertexBuffer
{
public:
  VertexBuffer(const VertexBuffer &) = default;

  VertexBuffer(VertexBuffer &&) = delete;

  VertexBuffer &operator=(const VertexBuffer &) = default;

  VertexBuffer &operator=(VertexBuffer &&) = delete;

  VertexBuffer() = default;

  virtual ~VertexBuffer() = default;

  virtual void Bind() const = 0;

  virtual void Unbind() const = 0;

  virtual void SetData(const void *data, size_t size) = 0;

  [[nodiscard]] virtual const BufferItemLayout &GetItemLayout() const = 0;

  virtual void SetItemLayout(const BufferItemLayout &layout) = 0;
};

class IndexBuffer
{
public:
  IndexBuffer(const IndexBuffer &) = default;

  IndexBuffer(IndexBuffer &&) = delete;

  IndexBuffer &operator=(const IndexBuffer &) = default;

  IndexBuffer &operator=(IndexBuffer &&) = delete;

  IndexBuffer() = default;

  virtual ~IndexBuffer() = default;

  virtual void Bind() const = 0;

  virtual void Unbind() const = 0;

  virtual void SetIndices(const uint32_t *indices, size_t count) = 0;

  virtual size_t GetCount() const = 0;
};

#endif // VERTEXBUFFER_H
