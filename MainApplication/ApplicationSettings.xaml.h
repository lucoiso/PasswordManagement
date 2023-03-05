// Author: Lucas Oliveira Vilas-Bôas
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

        Windows::Foundation::IAsyncAction InitializeSettingsProperties();
        void SaveSettings();
    };
}

namespace winrt::MainApplication::factory_implementation
{
    struct ApplicationSettings : ApplicationSettingsT<ApplicationSettings, implementation::ApplicationSettings>
    {
    };
}
