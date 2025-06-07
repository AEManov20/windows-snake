//
// Created by alex on 12/23/24.
//

#ifndef AUDIOBUFFER_H
#define AUDIOBUFFER_H

#include "ChannelLayout.h"

#include <cstdint>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
#include <algorithm>

struct SignalSpec
{
    uint32_t m_Rate;
    ChannelLayout m_Channels;
};

enum class AudioEncoding
{
    UInt8,
    UInt16,
    UInt24,
    UInt32,
    Int8,
    Int16,
    Int24,
    Int32,
    Float32,
    Float64
};

inline size_t GetEffectiveEncodingSize(AudioEncoding encoding)
{
    switch (encoding)
    {
    case AudioEncoding::UInt8:
    case AudioEncoding::Int8:
        return 1;
    case AudioEncoding::UInt16:
    case AudioEncoding::Int16:
        return 2;
    case AudioEncoding::UInt24:
    case AudioEncoding::Int24:
        return 3;
    case AudioEncoding::Float32:
    case AudioEncoding::UInt32:
    case AudioEncoding::Int32:
        return 4;
    case AudioEncoding::Float64:
        return 8;
    default:
        return 0;
    }
}

// Buffer which contains Linear PCM samples.
class AudioBuffer
{
public:
    // static AudioBuffer AudioBufferFromArray(const uint8_t* arr, size_t size, SignalSpec spec, AudioEncoding encoding)
    // {
    //     if (size % spec.m_Channels.Count() != 0) {
    //         throw std::runtime_error("Passed buffer's size (" +
    //             std::to_string(size) +
    //             ") isn't a multiple of the channels' count (" +
    //             std::to_string(spec.m_Channels.Count()) +
    //             ")");
    //     }

    //     std::vector<uint8_t> buffer;
    //     buffer.reserve(size);

    //     std::copy_n(arr, size, std::back_inserter(buffer));

    //     return { buffer, spec, encoding };
    // }

    AudioBuffer(std::vector<uint8_t> _buffer, const SignalSpec _spec, const AudioEncoding _encoding)
        : m_Buffer(std::move(_buffer)), m_Encoding(_encoding), m_Spec(_spec) {}

    AudioBuffer(size_t duration, SignalSpec spec);

    ~AudioBuffer() = default;

    [[nodiscard]] SignalSpec Spec() const noexcept { return m_Spec; }
    [[nodiscard]] AudioEncoding Encoding() const noexcept { return m_Encoding; }

    [[nodiscard]] size_t BufferLength() const noexcept { return m_Buffer.size(); }

    [[nodiscard]] const uint8_t* Data() const noexcept { return m_Buffer.data(); }
    [[nodiscard]] const std::vector<uint8_t>& Vector() const noexcept { return m_Buffer; }

private:
    std::vector<uint8_t> m_Buffer;
    AudioEncoding m_Encoding;
    SignalSpec m_Spec;
};

#endif //AUDIOBUFFER_H
