//
// Created by alex on 12/23/24.
//

#include "ChannelLayout.h"

ChannelLayout::ChannelLayout(ChannelLayoutType layout) noexcept
{
    switch (layout)
    {
        case ChannelLayoutType::MONO:
            m_Flags = std::bitset<32>(static_cast<uint32_t>(ChannelFlagValue::FRONT_LEFT));
        break;
        case ChannelLayoutType::STEREO:
            m_Flags = std::bitset<32>(static_cast<uint32_t>(
                static_cast<uint32_t>(ChannelFlagValue::FRONT_LEFT) | static_cast<uint32_t>(ChannelFlagValue::FRONT_RIGHT)));
        break;
        case ChannelLayoutType::TWO_POINT_FIVE:
            m_Flags = std::bitset<32>(static_cast<uint32_t>(
                static_cast<uint32_t>(ChannelFlagValue::FRONT_LEFT) | static_cast<uint32_t>(ChannelFlagValue::FRONT_RIGHT) |
                static_cast<uint32_t>(ChannelFlagValue::LFE1)));
        break;
        case ChannelLayoutType::FIVE_POINT_FIVE:
            m_Flags = std::bitset<32>(static_cast<uint32_t>(
                static_cast<uint32_t>(ChannelFlagValue::FRONT_LEFT) | static_cast<uint32_t>(ChannelFlagValue::FRONT_RIGHT) |
                static_cast<uint32_t>(ChannelFlagValue::REAR_LEFT) | static_cast<uint32_t>(ChannelFlagValue::REAR_RIGHT) |
                static_cast<uint32_t>(ChannelFlagValue::FRONT_CENTRE) | static_cast<uint32_t>(ChannelFlagValue::LFE1)));
        break;
    };
}

void ChannelLayout::SetFlagState(ChannelFlagValue f, bool state) noexcept
{
    if (state)
    {
        m_Flags |= static_cast<uint32_t>(f);
    }
    else
    {
        auto t = static_cast<uint32_t>(f);
        auto tmp = m_Flags;
        tmp &= t;
        m_Flags ^= tmp;
    }
}

bool ChannelLayout::GetFlagState(ChannelFlagValue f) const noexcept
{
    return (static_cast<uint32_t>(f) & m_Flags.to_ulong()) != 0;
}

size_t ChannelLayout::Count() const noexcept
{
    return m_Flags.count();
}

std::vector<ChannelFlagValue> ChannelLayout::GetAllEnabledFlags() const
{
    std::vector<ChannelFlagValue> out;

    for (auto it = ALL_CHANNEL_FLAG_VALUES.begin(); it != ALL_CHANNEL_FLAG_VALUES.end(); ++it)
    {
        auto flagValue = *it;
        if (GetFlagState(flagValue))
        {
            out.push_back(flagValue);
        }
    }

    return out;
}

