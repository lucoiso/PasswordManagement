// Author: Lucas Oliveira Vilas-Bôas
// Year: 2023
// Repository: https://github.com/lucoiso/PasswordManagement

#pragma once

#ifndef LUPASS_CASTING_HELPER_H
#define LUPASS_CASTING_HELPER_H

#include "pch.h"
#include <chrono>

using namespace winrt::MainApplication;

namespace winrt::Helper
{
    template <typename ParentTy>
    inline ParentTy GetParent(Microsoft::UI::Xaml::DependencyObject child_object)
    {
        if (!child_object)
        {
            return nullptr;
        }

        auto parent = Microsoft::UI::Xaml::Media::VisualTreeHelper::GetParent(child_object);
        if (!parent)
        {
            return nullptr;
        }

        if (auto casted_parent = parent.try_as<ParentTy>(); casted_parent)
        {
            return casted_parent;
        }

        return GetParent<ParentTy>(parent);
    }

    constexpr uint64_t s_ticks_per_day = 864000000000;
    constexpr uint64_t s_epoch_diff = 116444736000000000;

    inline winrt::clock::time_point ToTimePoint(const std::uint64_t& value)
    {
        const uint64_t time_count = value - s_epoch_diff;
        const auto duration = std::chrono::duration<uint64_t, std::ratio<1, 10'000'000>>(time_count);
        const auto time_point = std::chrono::time_point<std::chrono::system_clock>(duration);

        return winrt::clock::from_sys(time_point);
    }

    inline uint64_t GetCurrentDayTimeCount()
    {
        return (winrt::clock::now().time_since_epoch().count() / (static_cast<uint64_t>(24 * 60 * 60) * 10000000) * (static_cast<uint64_t>(24 * 60 * 60) * 10000000)) + s_ticks_per_day;
    }

    inline hstring TimeToString(const Windows::Foundation::DateTime& time, const bool include_date = true, const bool include_time = true)
    {
        hstring output;

        if (include_date)
        {
            const auto date_formatter = Windows::Globalization::DateTimeFormatting::DateTimeFormatter::ShortDate();
            output = output + date_formatter.Format(time);
        }

        if (include_time)
        {
            if (!output.empty())
            {
                output = output + L" - ";
            }

			const auto time_formatter = Windows::Globalization::DateTimeFormatting::DateTimeFormatter::ShortTime();
            output = output + time_formatter.Format(time);
		}

        return output;
    }
}
#endif
