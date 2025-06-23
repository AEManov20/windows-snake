
#include "AudioSource.h"
#include <cmath>

// this function will progressively generate one second of sine
std::optional<AudioBuffer> SourceSine::NextFrame()
{
    std::vector<float> outBuffer{};
    
    for (uint32_t i = 0; i < m_SignalSpec.m_Rate * m_SignalSpec.m_Channels.Count(); ++i)
    {
        // current angle
        float theta = static_cast<float>(i) / static_cast<float>(m_SignalSpec.m_Rate);

        outBuffer.push_back(sinf(m_Frequency * theta * M_PIf));
    }

    // m_ThetaRemainder = sinf(asinf(m_ThetaRemainder) * m_Frequency * M_PIf);

    return AudioBuffer(reinterpret_cast<const std::vector<uint8_t>&>(outBuffer), m_SignalSpec, AudioEncoding::Float32);
}
