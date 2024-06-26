// Author: Lucas Oliveira Vilas-B�as
// Year: 2023
// Repository: https://github.com/lucoiso/PasswordManagement

#pragma once

#include "ApplicationSettings.g.h"

namespace winrt::MainApplication::implementation
{
    struct ApplicationSettings : ApplicationSettingsT<ApplicationSettings>
    {
        ApplicationSettings();

        void Panel_Loaded(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args);
        Windows::Foundation::IAsyncAction BP_Reset_Click(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args);

        Windows::Foundation::IAsyncAction InitializeSettingsProperties();
        Windows::Foundation::IAsyncOperation<bool> SaveSettings();
    };
}

namespace winrt::MainApplication::factory_implementation
{
    struct ApplicationSettings : ApplicationSettingsT<ApplicationSettings, implementation::ApplicationSettings>
    {
    };
}
