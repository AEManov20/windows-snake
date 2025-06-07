//
// Created by alex on 12/23/24.
//

#ifndef AUDIOSOURCE_H
#define AUDIOSOURCE_H

#include "AudioBuffer.h"
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include <sstream>

class AudioSource
{
public:
    virtual ~AudioSource() = default;

    virtual SignalSpec Spec() = 0;
    virtual AudioEncoding Encoding() = 0;

    virtual std::optional<size_t> TotalSamples() = 0;
    virtual std::optional<size_t> CurrentSample() = 0;

    virtual std::optional<AudioBuffer> NextFrame() = 0;

    virtual bool IsInfallible() = 0;
};

class SourceResampler : AudioSource
{
public:
    SourceResampler(std::shared_ptr<AudioSource> source, SignalSpec targetSpec);

    SignalSpec Spec() override { return m_Spec; }
    AudioEncoding Encoding() override { return m_Source->Encoding(); }

    std::optional<size_t> TotalSamples() override;
    std::optional<size_t> CurrentSample() override;

    std::optional<AudioBuffer> NextFrame() override;

    bool IsInfallible() override { return m_Source->IsInfallible(); }

private:
    SignalSpec m_Spec;
    std::shared_ptr<AudioSource> m_Source;
};

class SourceReencoder : public AudioSource
{
public:
    SourceReencoder(std::shared_ptr<AudioSource> source, AudioEncoding targetEncoding);

    SignalSpec Spec() override { return m_Source->Spec(); }
    AudioEncoding Encoding() override { return m_Encoding; }

    std::optional<size_t> TotalSamples() override;
    std::optional<size_t> CurrentSample() override;

    std::optional<AudioBuffer> NextFrame() override;

    bool IsInfallible() override { return m_Source->IsInfallible(); }
private:
    AudioEncoding m_Encoding;
    std::shared_ptr<AudioSource> m_Source;
};

class SourceSine : public AudioSource
{
public:
    explicit SourceSine(SignalSpec spec, float frequency)
        : m_SignalSpec(spec), m_Frequency(frequency) {}

    SignalSpec Spec() { return m_SignalSpec; }
    AudioEncoding Encoding() { return AudioEncoding::Float32; }

    std::optional<size_t> TotalSamples() { return std::nullopt; }
    std::optional<size_t> CurrentSample() { return std::nullopt; }

    std::optional<AudioBuffer> NextFrame();

    bool IsInfallible() { return true; }

private:
    float m_Frequency;
    float m_ThetaRemainder = 0.f;

    SignalSpec m_SignalSpec;
};

#endif //AUDIOSOURCE_H
