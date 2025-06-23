//
// Created by alex on 12/23/24.
//

#include "SampleConversions.h"
#include <cassert>
#include <cstdint>
#include <algorithm> // for std::clamp
#include <iostream> // for std::cout, std::endl

/*!
    \brief Implements a function to convert between signed to unsigned of same bit count
    \param name name of the implemented function
    \param st signed type
    \param ut unsigned type
    \param smid signed type's midpoint
*/
#define IMPL_S_TO_U(name, st, ut, smid)                                            \
    inline ut name(st s)                                                           \
    {                                                                              \
        static_assert(sizeof(st) == sizeof(ut), "Types are not of the same size"); \
        ut out = static_cast<ut>(s);                                               \
        out += smid;                                                               \
        return out;                                                                \
    }

/*!
    \brief Implements a sample conversion function
    \param from source type
    \param to destination type
    \param sample name of sample parameter in the function
    \param func the function that will process the sample
*/
#define IMPL_CONVERT(from, to, sample, func) \
    template<>                               \
    to ConvertSample(from sample)            \
    {                                        \
        return func;                         \
    }

template <typename S, typename D>
D ConvertSample(S _)
{
    // default fallback implementation, when ConvertSample gets invoked
    // on non-implemented types
    static_assert(false, "These type arguments are not implemented yet");
    return D {};
}

// NOTE: a lot of the conversions (unsigned to signed in particular) take advantage of the unsigned types'
//       underflow behaviour in C++

// NOTE: there are conversions from a given type to the same type to completely fill the conversion matrix

// int8_t to ...

IMPL_S_TO_U(s8_to_u8, int8_t, uint8_t, 0x80)

IMPL_CONVERT(int8_t, uint8_t, s, s8_to_u8(s))
IMPL_CONVERT(int8_t, uint16_t, s, static_cast<uint16_t>(s8_to_u8(s)) << 8)
IMPL_CONVERT(int8_t, UInt24, s, UInt24(s8_to_u8(s)) << 16)
IMPL_CONVERT(int8_t, uint32_t, s, static_cast<uint32_t>(s8_to_u8(s)) << 24)

IMPL_CONVERT(int8_t, int8_t, s, s)
IMPL_CONVERT(int8_t, int16_t, s, static_cast<uint16_t>(s) << 8)
IMPL_CONVERT(int8_t, Int24, s, Int24(s) << 16)
IMPL_CONVERT(int8_t, int32_t, s, static_cast<uint32_t>(s) << 24)

IMPL_CONVERT(int8_t, float, s, static_cast<float>(s) / 128.f)
IMPL_CONVERT(int8_t, double, s, static_cast<double>(s) / 128.)

// int16_t to ...

IMPL_S_TO_U(s16_to_u16, int16_t, uint16_t, 0x8000)

IMPL_CONVERT(int16_t, uint8_t, s, static_cast<uint8_t>(s16_to_u16(s) >> 8))
IMPL_CONVERT(int16_t, uint16_t, s, s16_to_u16(s))
IMPL_CONVERT(int16_t, UInt24, s, UInt24(s16_to_u16(s)) << 8)
IMPL_CONVERT(int16_t, uint32_t, s, static_cast<uint32_t>(s16_to_u16(s)) << 16)

IMPL_CONVERT(int16_t, int8_t, s, static_cast<int8_t>(s >> 8))
IMPL_CONVERT(int16_t, int16_t, s, s)
IMPL_CONVERT(int16_t, Int24, s, Int24(s) << 8)
IMPL_CONVERT(int16_t, int32_t, s, static_cast<int32_t>(s) << 16)

IMPL_CONVERT(int16_t, float, s, static_cast<float>(s) / 32'768.f)
IMPL_CONVERT(int16_t, double, s, static_cast<double>(s) / 32'768.)

// int24_t to ...

IMPL_S_TO_U(s24_to_u24, Int24, UInt24, 0x80'0000)

IMPL_CONVERT(Int24, uint8_t, s, static_cast<uint8_t>(s24_to_u24(s) >> 16))
IMPL_CONVERT(Int24, uint16_t, s, static_cast<uint16_t>(s24_to_u24(s) >> 8))
IMPL_CONVERT(Int24, UInt24, s, s24_to_u24(s))
IMPL_CONVERT(Int24, uint32_t, s, static_cast<uint32_t>(s24_to_u24(s)) << 8)

IMPL_CONVERT(Int24, int8_t, s, static_cast<int8_t>(s >> 16))
IMPL_CONVERT(Int24, int16_t, s, static_cast<int16_t>(s >> 8))
IMPL_CONVERT(Int24, Int24, s, s)
IMPL_CONVERT(Int24, int32_t, s, static_cast<uint32_t>(s) << 8)

IMPL_CONVERT(Int24, float, s, static_cast<float>(s) / 8'388'608.f)
IMPL_CONVERT(Int24, double, s, static_cast<double>(s) / 8'388'608.)

// int32_t to ...

IMPL_S_TO_U(s32_to_u32, int32_t, uint32_t, 0x8000'0000)

IMPL_CONVERT(int32_t, uint8_t, s, static_cast<uint8_t>(s32_to_u32(s) >> 24))
IMPL_CONVERT(int32_t, uint16_t, s, static_cast<uint16_t>(s32_to_u32(s) >> 16))
IMPL_CONVERT(int32_t, UInt24, s, UInt24(s32_to_u32(s) >> 8))
IMPL_CONVERT(int32_t, uint32_t, s, s32_to_u32(s))

IMPL_CONVERT(int32_t, int8_t, s, static_cast<int8_t>(s >> 24))
IMPL_CONVERT(int32_t, int16_t, s, static_cast<int16_t>(s >> 16))
IMPL_CONVERT(int32_t, Int24, s, Int24(s >> 8))
IMPL_CONVERT(int32_t, int32_t, s, s)

IMPL_CONVERT(int32_t, float, s, static_cast<float>(static_cast<double>(s) / 2'147'483'648.))
IMPL_CONVERT(int32_t, double, s, static_cast<double>(s) / 2'147'483'648.)

// uint8_t to ...

IMPL_CONVERT(uint8_t, uint8_t, s, s)
IMPL_CONVERT(uint8_t, uint16_t, s, static_cast<uint16_t>(s) << 8)
IMPL_CONVERT(uint8_t, UInt24, s, UInt24(s) << 16)
IMPL_CONVERT(uint8_t, uint32_t, s, static_cast<uint32_t>(s) << 24)

IMPL_CONVERT(uint8_t, int8_t, s, static_cast<int8_t>(s - 0x80))
IMPL_CONVERT(uint8_t, int16_t, s, static_cast<int16_t>(static_cast<int8_t>(s - 0x80)) << 8)
IMPL_CONVERT(uint8_t, Int24, s, Int24(s - 0x80) << 16)
IMPL_CONVERT(uint8_t, int32_t, s, static_cast<int32_t>(static_cast<int8_t>(s - 0x80)) << 24)

IMPL_CONVERT(uint8_t, float, s, (static_cast<float>(s) / 128.f) - 1.f)
IMPL_CONVERT(uint8_t, double, s, (static_cast<double>(s) / 128.) - 1.)

// uint16_t to ...

IMPL_CONVERT(uint16_t, uint8_t, s, static_cast<uint8_t>(s >> 8))
IMPL_CONVERT(uint16_t, uint16_t, s, s)
IMPL_CONVERT(uint16_t, UInt24, s, UInt24(s) << 8)
IMPL_CONVERT(uint16_t, uint32_t, s, static_cast<uint32_t>(s) << 16)

IMPL_CONVERT(uint16_t, int8_t, s, static_cast<int8_t>((s - 0x8000) >> 8))
IMPL_CONVERT(uint16_t, int16_t, s, static_cast<int16_t>(s - 0x8000))
IMPL_CONVERT(uint16_t, Int24, s, Int24(s - 0x8000) << 8)
IMPL_CONVERT(uint16_t, int32_t, s, static_cast<int32_t>(static_cast<int16_t>(s - 0x8000)) << 16)

IMPL_CONVERT(uint16_t, float, s, (static_cast<float>(s) / 32'768.f) - 1.f)
IMPL_CONVERT(uint16_t, double, s, (static_cast<double>(s) / 32'768.) - 1.)

// uint24_t to ...

IMPL_CONVERT(UInt24, uint8_t, s, static_cast<uint8_t>(s >> 16))
IMPL_CONVERT(UInt24, uint16_t, s, static_cast<uint16_t>(s >> 8))
IMPL_CONVERT(UInt24, UInt24, s, s)
IMPL_CONVERT(UInt24, uint32_t, s, static_cast<uint32_t>(s) << 8)

IMPL_CONVERT(UInt24, int8_t, s, static_cast<int8_t>((s - 0x800000) >> 16))
IMPL_CONVERT(UInt24, int16_t, s, static_cast<int16_t>((s - 0x800000) >> 8))
IMPL_CONVERT(UInt24, Int24, s, Int24(s - 0x800000))
IMPL_CONVERT(UInt24, int32_t, s, static_cast<int32_t>((s - 0x800000)) << 8)

IMPL_CONVERT(UInt24, float, s, (static_cast<float>(s) / 8'388'608.f) - 1.f)
IMPL_CONVERT(UInt24, double, s, (static_cast<double>(s) / 8'388'608.) - 1.)

// uint32_t to ...

IMPL_CONVERT(uint32_t, uint8_t, s, static_cast<uint8_t>(s >> 24))
IMPL_CONVERT(uint32_t, uint16_t, s, static_cast<uint16_t>(s >> 16))
IMPL_CONVERT(uint32_t, UInt24, s, UInt24(s >> 8))
IMPL_CONVERT(uint32_t, uint32_t, s, s)

IMPL_CONVERT(uint32_t, int8_t, s, static_cast<int8_t>((s - 0x8000'0000) >> 24))
IMPL_CONVERT(uint32_t, int16_t, s, static_cast<int16_t>((s - 0x8000'0000) >> 16))
IMPL_CONVERT(uint32_t, Int24, s, Int24((s - 0x8000'0000) >> 8))
IMPL_CONVERT(uint32_t, int32_t, s, static_cast<int32_t>(s - 0x8000'0000))

IMPL_CONVERT(uint32_t, float, s, static_cast<float>((static_cast<double>(s) / 2'147'483'648.) - 1.))
IMPL_CONVERT(uint32_t, double, s, (static_cast<double>(s) / 2'147'483'648.) - 1.)

// float to ...

#define CLAMP_F32(s) std::clamp(s, -1.f, 1.f)

IMPL_CONVERT(float, uint8_t, s, static_cast<uint8_t>(std::clamp((CLAMP_F32(s) + 1.f) * 128.f, 0.f, 255.f)))
IMPL_CONVERT(float, uint16_t, s, static_cast<uint16_t>(std::clamp((CLAMP_F32(s) + 1.f) * 32'768.f, 0.f, 65'535.f)))
IMPL_CONVERT(float, UInt24, s, UInt24(std::clamp((CLAMP_F32(s) + 1.f) * 8'388'608.f, 0.f, 16'777'215.f)))
IMPL_CONVERT(float, uint32_t, s, static_cast<uint32_t>(std::clamp((static_cast<double>(CLAMP_F32(s) + 1.f)) * 2'147'483'648., 0., 4'294'967'295.)))

IMPL_CONVERT(float, int8_t, s, static_cast<int8_t>(std::clamp(CLAMP_F32(s) * 128.f, -128.f, 127.f)))
IMPL_CONVERT(float, int16_t, s, static_cast<int16_t>(std::clamp(CLAMP_F32(s) * 32'768.f, -32'768.f, 32'767.f)))
IMPL_CONVERT(float, Int24, s, Int24(std::clamp(CLAMP_F32(s) * 8'388'608.f, -8'388'608.f, 8'388'607.f)))
IMPL_CONVERT(float, int32_t, s, static_cast<int32_t>(std::clamp<int64_t>(CLAMP_F32(s) * 2'147'483'648.f, -2'147'483'648, 2'147'483'647)))

IMPL_CONVERT(float, float, s, s)
IMPL_CONVERT(float, double, s, s)

// double to ...

#define CLAMP_F64(s) std::clamp(s, -1., 1.)

IMPL_CONVERT(double, uint8_t, s, static_cast<uint8_t>(std::clamp((CLAMP_F64(s) + 1.) * 128., 0., 255.)))
IMPL_CONVERT(double, uint16_t, s, static_cast<uint16_t>(std::clamp((CLAMP_F64(s) + 1.) * 32'768., 0., 65'535.)))
IMPL_CONVERT(double, UInt24, s, UInt24(std::clamp((CLAMP_F64(s) + 1.) * 8'388'608., 0., 16'777'215.)))
IMPL_CONVERT(double, uint32_t, s, static_cast<uint32_t>(std::clamp((CLAMP_F64(s) + 1.) * 2'147'483'648., 0., 4'294'967'295.)))

IMPL_CONVERT(double, int8_t, s, static_cast<int8_t>(std::clamp(CLAMP_F64(s) * 128., -128., 127.)))
IMPL_CONVERT(double, int16_t, s, static_cast<int16_t>(std::clamp(CLAMP_F64(s) * 32'768., -32'768., 32'767.)))
IMPL_CONVERT(double, Int24, s, Int24(std::clamp(CLAMP_F64(s) * 8'388'608., -8'388'608., 8'388'607.)))
IMPL_CONVERT(double, int32_t, s, static_cast<int32_t>(std::clamp(CLAMP_F64(s) * 2'147'483'648., -2'147'483'648., 2'147'483'647.)))

IMPL_CONVERT(double, float, s, static_cast<float>(s))
IMPL_CONVERT(double, double, s, s)

#define TEST(x) std::cout << "Running " << #x << std::endl; x;

#define CONVERT_SAMPLE_TEST(from, to, fmax, fmid, fmin, tmax, tmid, tmin) \
    TEST(assert((ConvertSample<from, to>(fmax) == tmax)));                \
    TEST(assert((ConvertSample<from, to>(fmid) == tmid)));                \
    TEST(assert((ConvertSample<from, to>(fmin) == tmin)));

void verify_uint8_t_conversions()
{
    CONVERT_SAMPLE_TEST(uint8_t,  uint8_t, UINT8_MAX,  UINT8_MID,  0,         UINT8_MAX, UINT8_MID, 0)
    CONVERT_SAMPLE_TEST(uint16_t, uint8_t, UINT16_MAX, UINT16_MID, 0,         UINT8_MAX, UINT8_MID, 0)
    CONVERT_SAMPLE_TEST(UInt24,   uint8_t, UINT24_MAX, UINT24_MID, 0,         UINT8_MAX, UINT8_MID, 0)
    CONVERT_SAMPLE_TEST(uint32_t, uint8_t, UINT32_MAX, UINT32_MID, 0,         UINT8_MAX, UINT8_MID, 0)
    CONVERT_SAMPLE_TEST(int8_t,   uint8_t, INT8_MAX,   INT8_MID,   INT8_MIN,  UINT8_MAX, UINT8_MID, 0)
    CONVERT_SAMPLE_TEST(int16_t,  uint8_t, INT16_MAX,  INT16_MID,  INT16_MIN, UINT8_MAX, UINT8_MID, 0)
    CONVERT_SAMPLE_TEST(Int24,    uint8_t, INT24_MAX,  INT24_MID,  INT24_MIN, UINT8_MAX, UINT8_MID, 0)
    CONVERT_SAMPLE_TEST(int32_t,  uint8_t, INT32_MAX,  INT32_MID,  INT32_MIN, UINT8_MAX, UINT8_MID, 0)
    CONVERT_SAMPLE_TEST(float,    uint8_t, F32_MAX,    F32_MID,    F32_MIN,   UINT8_MAX, UINT8_MID, 0)
    CONVERT_SAMPLE_TEST(double,   uint8_t, F64_MAX,    F64_MID,    F64_MIN,   UINT8_MAX, UINT8_MID, 0)
}

void verify_uint16_t_conversions()
{
    CONVERT_SAMPLE_TEST(uint8_t,  uint16_t, UINT8_MAX,  UINT8_MID,  0,         UINT16_MAX - 255, UINT16_MID, 0)
    CONVERT_SAMPLE_TEST(uint16_t, uint16_t, UINT16_MAX, UINT16_MID, 0,         UINT16_MAX,       UINT16_MID, 0)
    CONVERT_SAMPLE_TEST(UInt24,   uint16_t, UINT24_MAX, UINT24_MID, 0,         UINT16_MAX,       UINT16_MID, 0)
    CONVERT_SAMPLE_TEST(uint32_t, uint16_t, UINT32_MAX, UINT32_MID, 0,         UINT16_MAX,       UINT16_MID, 0)
    CONVERT_SAMPLE_TEST(int8_t,   uint16_t, INT8_MAX,   INT8_MID,   INT8_MIN,  UINT16_MAX - 255, UINT16_MID, 0)
    CONVERT_SAMPLE_TEST(int16_t,  uint16_t, INT16_MAX,  INT16_MID,  INT16_MIN, UINT16_MAX,       UINT16_MID, 0)
    CONVERT_SAMPLE_TEST(Int24,    uint16_t, INT24_MAX,  INT24_MID,  INT24_MIN, UINT16_MAX,       UINT16_MID, 0)
    CONVERT_SAMPLE_TEST(int32_t,  uint16_t, INT32_MAX,  INT32_MID,  INT32_MIN, UINT16_MAX,       UINT16_MID, 0)
    CONVERT_SAMPLE_TEST(float,    uint16_t, F32_MAX,    F32_MID,    F32_MIN,   UINT16_MAX,       UINT16_MID, 0)
    CONVERT_SAMPLE_TEST(double,   uint16_t, F64_MAX,    F64_MID,    F64_MIN,   UINT16_MAX,       UINT16_MID, 0)
}

void verify_uint24_t_conversions()
{
    CONVERT_SAMPLE_TEST(uint8_t,  UInt24, UINT8_MAX,  UINT8_MID,  0,         UINT24_MAX - 65'535,    UINT24_MID, 0)
    CONVERT_SAMPLE_TEST(uint16_t, UInt24, UINT16_MAX, UINT16_MID, 0,         UINT24_MAX - 255,       UINT24_MID, 0)
    CONVERT_SAMPLE_TEST(UInt24,   UInt24, UINT24_MAX, UINT24_MID, 0,         UINT24_MAX,             UINT24_MID, 0)
    CONVERT_SAMPLE_TEST(uint32_t, UInt24, UINT32_MAX, UINT32_MID, 0,         UINT24_MAX,             UINT24_MID, 0)
    CONVERT_SAMPLE_TEST(int8_t,   UInt24, INT8_MAX,   INT8_MID,   INT8_MIN,  UINT24_MAX - 65'535,    UINT24_MID, 0)
    CONVERT_SAMPLE_TEST(int16_t,  UInt24, INT16_MAX,  INT16_MID,  INT16_MIN, UINT24_MAX - 255,       UINT24_MID, 0)
    CONVERT_SAMPLE_TEST(Int24,    UInt24, INT24_MAX,  INT24_MID,  INT24_MIN, UINT24_MAX,             UINT24_MID, 0)
    CONVERT_SAMPLE_TEST(int32_t,  UInt24, INT32_MAX,  INT32_MID,  INT32_MIN, UINT24_MAX,             UINT24_MID, 0)
    CONVERT_SAMPLE_TEST(float,    UInt24, F32_MAX,    F32_MID,    F32_MIN,   UINT24_MAX,             UINT24_MID, 0)
    CONVERT_SAMPLE_TEST(double,   UInt24, F64_MAX,    F64_MID,    F64_MIN,   UINT24_MAX,             UINT24_MID, 0)
}

void verify_uint32_t_conversions()
{
    CONVERT_SAMPLE_TEST(uint8_t,  uint32_t, UINT8_MAX,  UINT8_MID,  0,         UINT32_MAX - 16'777'215, UINT32_MID, 0)
    CONVERT_SAMPLE_TEST(uint16_t, uint32_t, UINT16_MAX, UINT16_MID, 0,         UINT32_MAX - 65'535,     UINT32_MID, 0)
    CONVERT_SAMPLE_TEST(UInt24,   uint32_t, UINT24_MAX, UINT24_MID, 0,         UINT32_MAX - 255,        UINT32_MID, 0)
    CONVERT_SAMPLE_TEST(uint32_t, uint32_t, UINT32_MAX, UINT32_MID, 0,         UINT32_MAX,              UINT32_MID, 0)
    CONVERT_SAMPLE_TEST(int8_t,   uint32_t, INT8_MAX,   INT8_MID,   INT8_MIN,  UINT32_MAX - 16'777'215, UINT32_MID, 0)
    CONVERT_SAMPLE_TEST(int16_t,  uint32_t, INT16_MAX,  INT16_MID,  INT16_MIN, UINT32_MAX - 65'535,     UINT32_MID, 0)
    CONVERT_SAMPLE_TEST(Int24,    uint32_t, INT24_MAX,  INT24_MID,  INT24_MIN, UINT32_MAX - 255,        UINT32_MID, 0)
    CONVERT_SAMPLE_TEST(int32_t,  uint32_t, INT32_MAX,  INT32_MID,  INT32_MIN, UINT32_MAX,              UINT32_MID, 0)
    CONVERT_SAMPLE_TEST(float,    uint32_t, F32_MAX,    F32_MID,    F32_MIN,   UINT32_MAX,              UINT32_MID, 0)
    CONVERT_SAMPLE_TEST(double,   uint32_t, F64_MAX,    F64_MID,    F64_MIN,   UINT32_MAX,              UINT32_MID, 0)
}

void verify_int8_t_conversions()
{
    CONVERT_SAMPLE_TEST(uint8_t,  int8_t, UINT8_MAX,  UINT8_MID,  0,         INT8_MAX, INT8_MID, INT8_MIN)
    CONVERT_SAMPLE_TEST(uint16_t, int8_t, UINT16_MAX, UINT16_MID, 0,         INT8_MAX, INT8_MID, INT8_MIN)
    CONVERT_SAMPLE_TEST(UInt24,   int8_t, UINT24_MAX, UINT24_MID, 0,         INT8_MAX, INT8_MID, INT8_MIN)
    CONVERT_SAMPLE_TEST(uint32_t, int8_t, UINT32_MAX, UINT32_MID, 0,         INT8_MAX, INT8_MID, INT8_MIN)
    CONVERT_SAMPLE_TEST(int8_t,   int8_t, INT8_MAX,   INT8_MID,   INT8_MIN,  INT8_MAX, INT8_MID, INT8_MIN)
    CONVERT_SAMPLE_TEST(int16_t,  int8_t, INT16_MAX,  INT16_MID,  INT16_MIN, INT8_MAX, INT8_MID, INT8_MIN)
    CONVERT_SAMPLE_TEST(Int24,    int8_t, INT24_MAX,  INT24_MID,  INT24_MIN, INT8_MAX, INT8_MID, INT8_MIN)
    CONVERT_SAMPLE_TEST(int32_t,  int8_t, INT32_MAX,  INT32_MID,  INT32_MIN, INT8_MAX, INT8_MID, INT8_MIN)
    CONVERT_SAMPLE_TEST(float,    int8_t, F32_MAX,    F32_MID,    F32_MIN,   INT8_MAX, INT8_MID, INT8_MIN)
    CONVERT_SAMPLE_TEST(double,   int8_t, F64_MAX,    F64_MID,    F64_MIN,   INT8_MAX, INT8_MID, INT8_MIN)
}

void verify_int16_t_conversions()
{
    CONVERT_SAMPLE_TEST(uint8_t,  int16_t, UINT8_MAX,  UINT8_MID,  0,         INT16_MAX - 255, INT16_MID, INT16_MIN)
    CONVERT_SAMPLE_TEST(uint16_t, int16_t, UINT16_MAX, UINT16_MID, 0,         INT16_MAX,       INT16_MID, INT16_MIN)
    CONVERT_SAMPLE_TEST(UInt24,   int16_t, UINT24_MAX, UINT24_MID, 0,         INT16_MAX,       INT16_MID, INT16_MIN)
    CONVERT_SAMPLE_TEST(uint32_t, int16_t, UINT32_MAX, UINT32_MID, 0,         INT16_MAX,       INT16_MID, INT16_MIN)
    CONVERT_SAMPLE_TEST(int8_t,   int16_t, INT8_MAX,   INT8_MID,   INT8_MIN,  INT16_MAX - 255, INT16_MID, INT16_MIN)
    CONVERT_SAMPLE_TEST(int16_t,  int16_t, INT16_MAX,  INT16_MID,  INT16_MIN, INT16_MAX,       INT16_MID, INT16_MIN)
    CONVERT_SAMPLE_TEST(Int24,    int16_t, INT24_MAX,  INT24_MID,  INT24_MIN, INT16_MAX,       INT16_MID, INT16_MIN)
    CONVERT_SAMPLE_TEST(int32_t,  int16_t, INT32_MAX,  INT32_MID,  INT32_MIN, INT16_MAX,       INT16_MID, INT16_MIN)
    CONVERT_SAMPLE_TEST(float,    int16_t, F32_MAX,    F32_MID,    F32_MIN,   INT16_MAX,       INT16_MID, INT16_MIN)
    CONVERT_SAMPLE_TEST(double,   int16_t, F64_MAX,    F64_MID,    F64_MIN,   INT16_MAX,       INT16_MID, INT16_MIN)
}

void verify_int24_t_conversions()
{
    CONVERT_SAMPLE_TEST(uint8_t,  Int24, UINT8_MAX,  UINT8_MID,  0,         INT24_MAX - 65'535, INT24_MID, INT24_MIN)
    CONVERT_SAMPLE_TEST(uint16_t, Int24, UINT16_MAX, UINT16_MID, 0,         INT24_MAX - 255,    INT24_MID, INT24_MIN)
    CONVERT_SAMPLE_TEST(UInt24,   Int24, UINT24_MAX, UINT24_MID, 0,         INT24_MAX,          INT24_MID, INT24_MIN)
    CONVERT_SAMPLE_TEST(uint32_t, Int24, UINT32_MAX, UINT32_MID, 0,         INT24_MAX,          INT24_MID, INT24_MIN)
    CONVERT_SAMPLE_TEST(int8_t,   Int24, INT8_MAX,   INT8_MID,   INT8_MIN,  INT24_MAX - 65'535, INT24_MID, INT24_MIN)
    CONVERT_SAMPLE_TEST(int16_t,  Int24, INT16_MAX,  INT16_MID,  INT16_MIN, INT24_MAX - 255,    INT24_MID, INT24_MIN)
    CONVERT_SAMPLE_TEST(Int24,    Int24, INT24_MAX,  INT24_MID,  INT24_MIN, INT24_MAX,          INT24_MID, INT24_MIN)
    CONVERT_SAMPLE_TEST(int32_t,  Int24, INT32_MAX,  INT32_MID,  INT32_MIN, INT24_MAX,          INT24_MID, INT24_MIN)
    CONVERT_SAMPLE_TEST(float,    Int24, F32_MAX,    F32_MID,    F32_MIN,   INT24_MAX,          INT24_MID, INT24_MIN)
    CONVERT_SAMPLE_TEST(double,   Int24, F64_MAX,    F64_MID,    F64_MIN,   INT24_MAX,          INT24_MID, INT24_MIN)
}

void verify_int32_t_conversions()
{
    CONVERT_SAMPLE_TEST(uint8_t,  int32_t, UINT8_MAX,  UINT8_MID,  0,         INT32_MAX - 16'777'215, INT32_MID, INT32_MIN)
    CONVERT_SAMPLE_TEST(uint16_t, int32_t, UINT16_MAX, UINT16_MID, 0,         INT32_MAX - 65'535,     INT32_MID, INT32_MIN)
    CONVERT_SAMPLE_TEST(UInt24,   int32_t, UINT24_MAX, UINT24_MID, 0,         INT32_MAX - 255,        INT32_MID, INT32_MIN)
    CONVERT_SAMPLE_TEST(uint32_t, int32_t, UINT32_MAX, UINT32_MID, 0,         INT32_MAX,              INT32_MID, INT32_MIN)
    CONVERT_SAMPLE_TEST(int8_t,   int32_t, INT8_MAX,   INT8_MID,   INT8_MIN,  INT32_MAX - 16'777'215, INT32_MID, INT32_MIN)
    CONVERT_SAMPLE_TEST(int16_t,  int32_t, INT16_MAX,  INT16_MID,  INT16_MIN, INT32_MAX - 65'535,     INT32_MID, INT32_MIN)
    CONVERT_SAMPLE_TEST(Int24,    int32_t, INT24_MAX,  INT24_MID,  INT24_MIN, INT32_MAX - 255,        INT32_MID, INT32_MIN)
    CONVERT_SAMPLE_TEST(int32_t,  int32_t, INT32_MAX,  INT32_MID,  INT32_MIN, INT32_MAX,              INT32_MID, INT32_MIN)
    CONVERT_SAMPLE_TEST(float,    int32_t, F32_MAX,    F32_MID,    F32_MIN,   INT32_MAX,              INT32_MID, INT32_MIN)
    CONVERT_SAMPLE_TEST(double,   int32_t, F64_MAX,    F64_MID,    F64_MIN,   INT32_MAX,              INT32_MID, INT32_MIN)
}

void verify_float_conversions()
{
    CONVERT_SAMPLE_TEST(uint8_t,  float, UINT8_MAX,  UINT8_MID,  0,         127.f           / 128.f,           0.f, -1.f)
    CONVERT_SAMPLE_TEST(uint16_t, float, UINT16_MAX, UINT16_MID, 0,         32'767.f        / 32'768.f,        0.f, -1.f)
    CONVERT_SAMPLE_TEST(UInt24,   float, UINT24_MAX, UINT24_MID, 0,         8'388'607.f     / 8'388'608.f,     0.f, -1.f)
    CONVERT_SAMPLE_TEST(uint32_t, float, UINT32_MAX, UINT32_MID, 0,         2'147'483'647.f / 2'147'483'648.f, 0.f, -1.f)
    CONVERT_SAMPLE_TEST(int8_t,   float, INT8_MAX,   INT8_MID,   INT8_MIN,  127.f           / 128.f,           0.f, -1.f)
    CONVERT_SAMPLE_TEST(int16_t,  float, INT16_MAX,  INT16_MID,  INT16_MIN, 32'767.f        / 32'768.f,        0.f, -1.f)
    CONVERT_SAMPLE_TEST(Int24,    float, INT24_MAX,  INT24_MID,  INT24_MIN, 8'388'607.f     / 8'388'608.f,     0.f, -1.f)
    CONVERT_SAMPLE_TEST(int32_t,  float, INT32_MAX,  INT32_MID,  INT32_MIN, 2'147'483'647.f / 2'147'483'648.f, 0.f, -1.f)

    CONVERT_SAMPLE_TEST(float,    float, F32_MAX,    F32_MID,    F32_MIN,   1.f, 0.f, -1.f)
    CONVERT_SAMPLE_TEST(double,   float, F64_MAX,    F64_MID,    F64_MIN,   1.f, 0.f, -1.f)
}

void verify_double_conversions()
{
    CONVERT_SAMPLE_TEST(uint8_t,  double, UINT8_MAX,  UINT8_MID,  0,         127.           / 128.,           0., -1.)
    CONVERT_SAMPLE_TEST(uint16_t, double, UINT16_MAX, UINT16_MID, 0,         32'767.        / 32'768.,        0., -1.)
    CONVERT_SAMPLE_TEST(UInt24,   double, UINT24_MAX, UINT24_MID, 0,         8'388'607.     / 8'388'608.,     0., -1.)
    CONVERT_SAMPLE_TEST(uint32_t, double, UINT32_MAX, UINT32_MID, 0,         2'147'483'647. / 2'147'483'648., 0., -1.)
    CONVERT_SAMPLE_TEST(int8_t,   double, INT8_MAX,   INT8_MID,   INT8_MIN,  127.           / 128.,           0., -1.)
    CONVERT_SAMPLE_TEST(int16_t,  double, INT16_MAX,  INT16_MID,  INT16_MIN, 32'767.        / 32'768.,        0., -1.)
    CONVERT_SAMPLE_TEST(Int24,    double, INT24_MAX,  INT24_MID,  INT24_MIN, 8'388'607.     / 8'388'608.,     0., -1.)
    CONVERT_SAMPLE_TEST(int32_t,  double, INT32_MAX,  INT32_MID,  INT32_MIN, 2'147'483'647. / 2'147'483'648., 0., -1.)

    CONVERT_SAMPLE_TEST(float,    double, F32_MAX,    F32_MID,    F32_MIN,   1., 0., -1.)
    CONVERT_SAMPLE_TEST(double,   double, F64_MAX,    F64_MID,    F64_MIN,   1., 0., -1.)
}


void verify_all_conversions()
{
    TEST(verify_uint8_t_conversions());
    TEST(verify_uint16_t_conversions());
    TEST(verify_uint24_t_conversions());
    TEST(verify_uint32_t_conversions());
    TEST(verify_int8_t_conversions());
    TEST(verify_int16_t_conversions());
    TEST(verify_int24_t_conversions());
    TEST(verify_int32_t_conversions());
    TEST(verify_float_conversions());
    TEST(verify_double_conversions());
}

