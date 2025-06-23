//
// Created by alex on 12/23/24.
//

#ifndef CHANNELS_H
#define CHANNELS_H

#include <cstdint>
#include <vector>
#include <bitset>

enum class ChannelLayoutType
{
    MONO,
    STEREO,
    TWO_POINT_FIVE,
    FIVE_POINT_FIVE
};

enum class ChannelFlagValue : uint32_t
{
    // Front-left (left) or the Mono channel.
    FRONT_LEFT = 0x00000001,
    // Front-right (right) channel.
    FRONT_RIGHT = 0x00000002,
    // Front-centre (centre) channel.
    FRONT_CENTRE = 0x00000004,
    // Low frequency channel 1.
    LFE1 = 0x00000008,
    // Rear-left (surround rear left) channel.
    REAR_LEFT = 0x00000010,
    // Rear-right (surround rear right) channel.
    REAR_RIGHT = 0x00000020,
    // Front left-of-centre (left center) channel.
    FRONT_LEFT_CENTRE = 0x00000040,
    // Front right-of-centre (right center) channel.
    FRONT_RIGHT_CENTRE = 0x00000080,
    // Rear-centre (surround rear centre) channel.
    REAR_CENTRE = 0x00000100,
    // Side left (surround left) channel.
    SIDE_LEFT = 0x00000200,
    // Side right (surround right) channel.
    SIDE_RIGHT = 0x00000400,
    // Top centre channel.
    TOP_CENTRE = 0x00000800,
    // Top front-left channel.
    TOP_FRONT_LEFT = 0x00001000,
    // Top centre channel.
    TOP_FRONT_CENTRE = 0x00002000,
    // Top front-right channel.
    TOP_FRONT_RIGHT = 0x00004000,
    // Top rear-left channel.
    TOP_REAR_LEFT = 0x00008000,
    // Top rear-centre channel.
    TOP_REAR_CENTRE = 0x00010000,
    // Top rear-right channel.
    TOP_REAR_RIGHT = 0x00020000,
    // Rear left-of-centre channel.
    REAR_LEFT_CENTRE = 0x00040000,
    // Rear right-of-centre channel.
    REAR_RIGHT_CENTRE = 0x00080000,
    // Front left-wide channel.
    FRONT_LEFT_WIDE = 0x00100000,
    // Front right-wide channel.
    FRONT_RIGHT_WIDE = 0x00200000,
    // Front left-high channel.
    FRONT_LEFT_HIGH = 0x00400000,
    // Front centre-high channel.
    FRONT_CENTRE_HIGH = 0x00800000,
    // Front right-high channel.
    FRONT_RIGHT_HIGH = 0x01000000,
    // Low frequency channel 2.
    LFE2 = 0x02000000,
};

const std::vector<ChannelFlagValue> ALL_CHANNEL_FLAG_VALUES = {
    ChannelFlagValue::FRONT_LEFT,        ChannelFlagValue::FRONT_RIGHT,
    ChannelFlagValue::FRONT_CENTRE,      ChannelFlagValue::LFE1,
    ChannelFlagValue::REAR_LEFT,         ChannelFlagValue::REAR_RIGHT,
    ChannelFlagValue::FRONT_LEFT_CENTRE, ChannelFlagValue::FRONT_RIGHT_CENTRE,
    ChannelFlagValue::REAR_CENTRE,       ChannelFlagValue::SIDE_LEFT,
    ChannelFlagValue::SIDE_RIGHT,        ChannelFlagValue::TOP_CENTRE,
    ChannelFlagValue::TOP_FRONT_LEFT,    ChannelFlagValue::TOP_FRONT_CENTRE,
    ChannelFlagValue::TOP_FRONT_RIGHT,   ChannelFlagValue::TOP_REAR_LEFT,
    ChannelFlagValue::TOP_REAR_CENTRE,   ChannelFlagValue::TOP_REAR_RIGHT,
    ChannelFlagValue::REAR_LEFT_CENTRE,  ChannelFlagValue::REAR_RIGHT_CENTRE,
    ChannelFlagValue::FRONT_LEFT_WIDE,   ChannelFlagValue::FRONT_RIGHT_WIDE,
    ChannelFlagValue::FRONT_LEFT_HIGH,   ChannelFlagValue::FRONT_CENTRE_HIGH,
    ChannelFlagValue::FRONT_RIGHT_HIGH,  ChannelFlagValue::LFE2
};

enum class ChannelDataType : uint8_t {
    UNSIGNED8,
    UNSIGNED16,
    UNSIGNED24,
    UNSIGNED32,
    SIGNED8,
    SIGNED16,
    SIGNED24,
    SIGNED32,
    FLOAT32,
    FLOAT64,
};

struct ChannelLayout
{
private:
    std::bitset<32> m_Flags;

public:
    explicit ChannelLayout(ChannelLayoutType init) noexcept;
    ChannelLayout() = default;
    ~ChannelLayout() = default;

    void SetFlagState(ChannelFlagValue f, bool state) noexcept;
    [[nodiscard]] bool GetFlagState(ChannelFlagValue f) const noexcept;
    [[nodiscard]] size_t Count() const noexcept;

    [[nodiscard]] std::vector<ChannelFlagValue> GetAllEnabledFlags() const;
};

#endif //CHANNELS_H
