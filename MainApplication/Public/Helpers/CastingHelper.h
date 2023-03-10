// Author: Lucas Oliveira Vilas-Bôas
// Year: 2023
// Repository: https://github.com/lucoiso/PasswordManagement

#pragma once

#ifndef LUPASS_CASTING_HELPER_H
#define LUPASS_CASTING_HELPER_H

#include "pch.h"

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

    inline winrt::clock::time_point ToTimePoint(const std::uint64_t& value) 
    {
        constexpr uint64_t epoch_diff = 116444736000000000;
        const uint64_t time_count = value - epoch_diff;

        const auto duration = std::chrono::duration<uint64_t, std::ratio<1, 10'000'000>>(time_count);
        const auto time_point = std::chrono::time_point<std::chrono::system_clock>(duration);

        return winrt::clock::from_sys(time_point);
    }

    inline hstring TimeToString(const Windows::Foundation::DateTime& time)
    {        
        const auto date_formatter = Windows::Globalization::DateTimeFormatting::DateTimeFormatter::ShortDate();
		const auto time_formatter = Windows::Globalization::DateTimeFormatting::DateTimeFormatter::ShortTime();

        return date_formatter.Format(time) + L" - " + time_formatter.Format(time);
    }
}
#endif
