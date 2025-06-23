//
// Created by alex on 12/23/24.
//

#ifndef OUTPUT_H
#define OUTPUT_H

#include "AudioBuffer.h"
#include <SDL3/SDL_audio.h>

class AudioOutput
{
public:
    virtual ~AudioOutput() = default;

    virtual void Write(const AudioBuffer&) = 0;
    virtual void Flush() = 0;

    virtual SignalSpec Spec() = 0;
    virtual AudioEncoding Encoding() = 0;
};

class SdlAudioOutput : public AudioOutput
{
public:
    SdlAudioOutput(SignalSpec spec, AudioEncoding encoding);

    SdlAudioOutput(SdlAudioOutput &&other) noexcept;

    SdlAudioOutput &operator=(SdlAudioOutput &&other) noexcept;

    SdlAudioOutput(const SdlAudioOutput &other) = delete;

    SdlAudioOutput &operator=(const SdlAudioOutput &) = delete;

    ~SdlAudioOutput() override;

    void Write(const AudioBuffer&) override;

    void Flush() override;

    [[nodiscard]] SignalSpec Spec() override { return m_Spec; }

    [[nodiscard]] AudioEncoding Encoding() override { return m_Encoding; }

private:
    SDL_AudioDeviceID m_Device;
    SDL_AudioStream *m_Stream;

    SignalSpec m_Spec;
    AudioEncoding m_Encoding;
};

#endif //OUTPUT_H
