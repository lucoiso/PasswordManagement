// Author: Lucas Oliveira Vilas-Bôas
// Year: 2023
// Repository: https://github.com/lucoiso/PasswordManagement

#pragma once

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
}