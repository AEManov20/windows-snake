//
// Created by alex on 12/23/24.
//

#include "AudioOutput.h"

#include <assert.h>

SDL_AudioFormat GetAudioFormat(AudioEncoding encoding)
{
    switch (encoding)
    {
    case AudioEncoding::UInt8:
        return SDL_AUDIO_U8;
    case AudioEncoding::Int8:
        return SDL_AUDIO_S8;
    case AudioEncoding::Int16:
        return SDL_AUDIO_S16;
    case AudioEncoding::Int32:
        return SDL_AUDIO_S32;
    case AudioEncoding::Float32:
        return SDL_AUDIO_F32;
    default:
        throw std::runtime_error("Unsupported encoding");
    }
}

SDL_AudioSpec CreateSpec(const SignalSpec spec, const AudioEncoding encoding)
{
    SDL_AudioSpec sdlSpec;
    sdlSpec.channels = static_cast<int32_t>(spec.m_Channels.Count());
    sdlSpec.freq = static_cast<int32_t>(spec.m_Rate);
    sdlSpec.format = GetAudioFormat(encoding);

    return sdlSpec;
}

SdlAudioOutput::SdlAudioOutput(SignalSpec spec, AudioEncoding encoding)
    : m_Spec(spec),
      m_Encoding(encoding)
{
    SDL_AudioSpec sdlSpec = CreateSpec(spec, encoding);

    m_Device = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &sdlSpec);
    m_Stream = SDL_CreateAudioStream(&sdlSpec, &sdlSpec);

    if (m_Device == 0)
    {
        throw std::runtime_error("Could not open audio device: " + std::string(SDL_GetError()));
    }

    if (m_Stream == nullptr)
    {
        SDL_CloseAudioDevice(m_Device);
        throw std::runtime_error("Could not create audio stream: " + std::string(SDL_GetError()));
    }

    if (!SDL_BindAudioStream(m_Device, m_Stream))
    {
        SDL_DestroyAudioStream(m_Stream);
        SDL_CloseAudioDevice(m_Device);
        throw std::runtime_error("Could not bind audio stream: " + std::string(SDL_GetError()));
    }
}

SdlAudioOutput::SdlAudioOutput(SdlAudioOutput &&other) noexcept
    : m_Device(std::exchange(other.m_Device, 0)),
      m_Stream(std::exchange(other.m_Stream, nullptr)),
      m_Spec(std::exchange(other.m_Spec, {})),
      m_Encoding(std::exchange(other.m_Encoding, AudioEncoding::UInt8))
{
}

SdlAudioOutput & SdlAudioOutput::operator=(SdlAudioOutput &&other) noexcept
{
    SdlAudioOutput tmp(std::move(other));

    std::swap(m_Device, tmp.m_Device);
    std::swap(m_Stream, tmp.m_Stream);
    std::swap(m_Spec, tmp.m_Spec);
    std::swap(m_Encoding, tmp.m_Encoding);

    return *this;
}

SdlAudioOutput::~SdlAudioOutput()
{
    if (m_Stream != nullptr)
        SDL_DestroyAudioStream(m_Stream);

    if (m_Device != 0)
        SDL_CloseAudioDevice(m_Device);
}

void SdlAudioOutput::Write(const AudioBuffer& audioBuffer)
{
    if (audioBuffer.Encoding() != m_Encoding)
        throw std::runtime_error("AudioBuffer encoding must match the output encoding");

    if (audioBuffer.Spec().m_Channels.Count() != m_Spec.m_Channels.Count())
        throw std::runtime_error("AudioBuffer channel layout must match the output channel layout");
    if (audioBuffer.Spec().m_Rate != m_Spec.m_Rate)
        throw std::runtime_error("AudioBuffer sample rate must match the output sample rate");

    SDL_PutAudioStreamData(m_Stream, audioBuffer.Data(), static_cast<int32_t>(audioBuffer.BufferLength()));
}

void SdlAudioOutput::Flush()
{
    SDL_FlushAudioStream(m_Stream);
}
