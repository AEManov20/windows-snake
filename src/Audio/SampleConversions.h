//
// Created by alex on 12/23/24.
//

#ifndef SAMPLECONVERSIONS_H
#define SAMPLECONVERSIONS_H

#include <cstdint>
#include <any>
#include <bit>

#include "AudioBuffer.h"

#define UINT8_MID static_cast<uint8_t>(128)
#define UINT16_MID static_cast<uint16_t>(32'768)
#define UINT24_MID UInt24(8'388'608)
#define UINT32_MID static_cast<uint32_t>(2'147'483'648)

#define INT8_MID static_cast<int8_t>(0)
#define INT16_MID static_cast<int16_t>(0)
#define INT24_MID Int24(0)
#define INT32_MID static_cast<int32_t>(0)

#define UINT24_MAX UInt24(16'777'215)
#define INT24_MAX Int24(8'388'607)
#define INT24_MIN Int24(-8'388'608)

#define F32_MAX 1.f
#define F32_MID 0.f
#define F32_MIN (-1.f)

#define F64_MAX 1.
#define F64_MID 0.
#define F64_MIN (-1.)

struct __attribute__((packed)) Int24 {
    int32_t value : 24;

    Int24(const int32_t val = 0) { value = val; }

    // convert to int32_t (with sign extension)
    operator int32_t() const { return (value << 8) >> 8; }
};

struct __attribute__((packed)) UInt24 {
    uint32_t value : 24;

    UInt24(const uint32_t val = 0) { value = val; }

    operator uint32_t() const { return value; }

    UInt24 &operator+=(const Int24 i) {
        value += i.value;
        return *this;
    };
};

template <typename S, typename D>
D ConvertSample(S src);

template <typename S, typename D>
std::vector<D> ConvertSampleVector(const std::vector<S>& src)
{
    std::vector<D> converted;
    converted.reserve(src.size());

    for (const auto& sample : src)
    {
        converted.push_back(ConvertSample<S, D>(sample));
    }

    return converted;
}

template <typename S>
std::vector<uint8_t> ConvertSampleVectorDynamic(const std::vector<S>& src, const AudioEncoding dstEncoding)
{
    switch (dstEncoding)
    {
    case AudioEncoding::UInt8:
        return ConvertSampleVector<S, uint8_t>(src);
    case AudioEncoding::UInt16:
    {
        auto conv = ConvertSampleVector<S, uint16_t>(src);
        return reinterpret_cast<std::vector<uint8_t>&>(conv);
    }
    case AudioEncoding::UInt24:
    {
        auto conv = ConvertSampleVector<S, UInt24>(src);
        return reinterpret_cast<std::vector<uint8_t>&>(conv);
    }
    case AudioEncoding::UInt32:
    {
        auto conv = ConvertSampleVector<S, uint32_t>(src);
        return reinterpret_cast<std::vector<uint8_t>&>(conv);
    }
    case AudioEncoding::Int8:
    {
        auto conv = ConvertSampleVector<S, int8_t>(src);
        return reinterpret_cast<std::vector<uint8_t>&>(conv);
    }
    case AudioEncoding::Int16:
    {
        auto conv = ConvertSampleVector<S, int16_t>(src);
        return reinterpret_cast<std::vector<uint8_t>&>(conv);
    }
    case AudioEncoding::Int24:
    {
        auto conv = ConvertSampleVector<S, Int24>(src);
        return reinterpret_cast<std::vector<uint8_t>&>(conv);
    }
    case AudioEncoding::Int32:
    {
        auto conv = ConvertSampleVector<S, int32_t>(src);
        return reinterpret_cast<std::vector<uint8_t>&>(conv);
    }
    case AudioEncoding::Float32:
    {
        auto conv = ConvertSampleVector<S, float>(src);
        return reinterpret_cast<std::vector<uint8_t>&>(conv);
    }
    case AudioEncoding::Float64:
    {
        auto conv = ConvertSampleVector<S, double>(src);
        return reinterpret_cast<std::vector<uint8_t>&>(conv);
    }
    default:
        throw std::runtime_error("Unsupported encoding");
    }
}

#endif //SAMPLECONVERSIONS_H
