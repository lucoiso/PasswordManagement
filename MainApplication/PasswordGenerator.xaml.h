// Author: Lucas Oliveira Vilas-Bôas
// Year: 2023
// Repository: https://github.com/lucoiso/PasswordManagement

#pragma once

#include "PasswordGenerator.g.h"

namespace winrt::MainApplication::implementation
{
    struct PasswordGenerator : PasswordGeneratorT<PasswordGenerator>
    {
        PasswordGenerator();

        void Panel_Loaded(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args);

        Windows::Foundation::IAsyncAction BP_Generate_Click(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args);
        void BP_Copy_Click(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args);
        void BP_Close_Click(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args);

        void InitializeSettingsProperties();

        event_token OnClose(MainApplication::SingleVoidDelegate const& handler);
        void OnClose(event_token const& token) noexcept;

    private:
        event<MainApplication::SingleVoidDelegate> m_close;
    };
}

namespace winrt::MainApplication::factory_implementation
{
    struct PasswordGenerator : PasswordGeneratorT<PasswordGenerator, implementation::PasswordGenerator>
    {
    };
}
