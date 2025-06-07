//
// Created by alex on 12/23/24.
//

#include "AudioBuffer.h"
#include "ChannelLayout.h"
#include "AudioSource.h"
#include <algorithm>
#include <iostream>
#include <memory>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

#include "SampleConversions.h"


SourceResampler::SourceResampler(std::shared_ptr<AudioSource> source, SignalSpec targetSpec)
    : m_Spec(targetSpec), m_Source(std::move(source))
{
}

std::optional<size_t> SourceResampler::TotalSamples()
{
    auto originalTotalSamples = m_Source->TotalSamples();

    if (!originalTotalSamples.has_value())
    {
        return std::nullopt;
    }

    const double origTotalSamplesDouble = static_cast<double>(originalTotalSamples.value());
    const double origSampleRate = m_Source->Spec().m_Rate;
    const double targetSampleRate = m_Spec.m_Rate;

    return static_cast<size_t>(origTotalSamplesDouble * (targetSampleRate / origSampleRate));
}

std::optional<size_t> SourceResampler::CurrentSample()
{
    auto originalCurrentSample = m_Source->CurrentSample();

    if (!originalCurrentSample.has_value())
    {
        return std::nullopt;
    }

    const double origCurrentSampleDouble = static_cast<double>(originalCurrentSample.value());
    const double origSampleRate = m_Source->Spec().m_Rate;
    const double targetSampleRate = m_Spec.m_Rate;

    return static_cast<size_t>(origCurrentSampleDouble * (targetSampleRate / origSampleRate));
}

std::optional<AudioBuffer> SourceResampler::NextFrame()
{
    // TODO:
    return std::nullopt;
}

SourceReencoder::SourceReencoder(std::shared_ptr<AudioSource> source, AudioEncoding targetEncoding)
    : m_Encoding(targetEncoding), m_Source(std::move(source))
{
}

std::optional<size_t> SourceReencoder::TotalSamples()
{
    return m_Source->TotalSamples();
}

std::optional<size_t> SourceReencoder::CurrentSample()
{
    return m_Source->CurrentSample();
}

std::optional<AudioBuffer> SourceReencoder::NextFrame()
{
    auto frame = m_Source->NextFrame();
    if (!frame.has_value())
        return std::nullopt;

    if (frame->Encoding() == m_Encoding)
        return frame;

    switch (frame->Encoding())
    {
        case AudioEncoding::UInt8:
            return AudioBuffer(ConvertSampleVectorDynamic(frame->Vector(), m_Encoding), m_Source->Spec(), m_Encoding);
        case AudioEncoding::Int8:
            return AudioBuffer(ConvertSampleVectorDynamic(reinterpret_cast<const std::vector<int8_t>&>(frame->Vector()), m_Encoding), m_Source->Spec(), m_Encoding);
        case AudioEncoding::UInt16:
            return AudioBuffer(ConvertSampleVectorDynamic(reinterpret_cast<const std::vector<uint16_t>&>(frame->Vector()), m_Encoding), m_Source->Spec(), m_Encoding);
        case AudioEncoding::Int16:
            return AudioBuffer(ConvertSampleVectorDynamic(reinterpret_cast<const std::vector<int16_t>&>(frame->Vector()), m_Encoding), m_Source->Spec(), m_Encoding);
        case AudioEncoding::UInt24:
            return AudioBuffer(ConvertSampleVectorDynamic(reinterpret_cast<const std::vector<UInt24>&>(frame->Vector()), m_Encoding), m_Source->Spec(), m_Encoding);
        case AudioEncoding::Int24:
            return AudioBuffer(ConvertSampleVectorDynamic(reinterpret_cast<const std::vector<Int24>&>(frame->Vector()), m_Encoding), m_Source->Spec(), m_Encoding);
        case AudioEncoding::UInt32:
            return AudioBuffer(ConvertSampleVectorDynamic(reinterpret_cast<const std::vector<uint32_t>&>(frame->Vector()), m_Encoding), m_Source->Spec(), m_Encoding);
        case AudioEncoding::Int32:
            return AudioBuffer(ConvertSampleVectorDynamic(reinterpret_cast<const std::vector<int32_t>&>(frame->Vector()), m_Encoding), m_Source->Spec(), m_Encoding);
        case AudioEncoding::Float32:
            return AudioBuffer(ConvertSampleVectorDynamic(reinterpret_cast<const std::vector<float>&>(frame->Vector()), m_Encoding), m_Source->Spec(), m_Encoding);
        case AudioEncoding::Float64:
            return AudioBuffer(ConvertSampleVectorDynamic(reinterpret_cast<const std::vector<double>&>(frame->Vector()), m_Encoding), m_Source->Spec(), m_Encoding);
        default:
            throw std::runtime_error("Unsupported encoding");
    }
}


// #define IMPL_NEXT_FRAME(TYPE, CAPITALIZED_TYPE)                                      \
//     std::optional<AudioBuffer<TYPE>> SourceMp3::NextFrame##CAPITALIZED_TYPE()        \
//     {                                                                                \
//         uint8_t *frameData;                                                          \
//         size_t bytes;                                                                \
//                                                                                      \
//         if (!DecodeNextFrame(&frameData, &bytes))                                    \
//             return std::nullopt;                                                     \
//                                                                                      \
//         return FrameToAudioBuffer<TYPE>(m_Encoding, frameData, bytes, this->Spec()); \
//     }
//
// IMPL_NEXT_FRAME(float, Float)
// IMPL_NEXT_FRAME(uint8_t, UInt8)
// IMPL_NEXT_FRAME(int8_t, Int8)
// IMPL_NEXT_FRAME(int16_t, Int16)
// IMPL_NEXT_FRAME(Int24, Int24)
// IMPL_NEXT_FRAME(int32_t, Int32)
