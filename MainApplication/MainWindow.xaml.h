// Author: Lucas Oliveira Vilas-Bôas
// Year: 2023
// Repository: https://github.com/lucoiso/PasswordManagement

#pragma once

#include "MainWindow.g.h"

namespace winrt::MainApplication::implementation
{
    struct MainWindow : MainWindowT<MainWindow>
    {
        MainWindow();
        
        Windows::Foundation::IAsyncAction FR_MainFrame_Loaded(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args);
        Windows::Foundation::IAsyncAction InitializeLicenseInformation();

        Windows::Foundation::IAsyncAction BT_Settings_Clicked(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args);
        Windows::Foundation::IAsyncAction OpenStorePage_Invoked(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args);

        template<typename UriTy>
        inline Windows::Foundation::IAsyncAction LaunchUri(const UriTy& uri)
        {
            LUPASS_LOG_FUNCTION();

            if constexpr (std::is_base_of<UriTy, hstring>::value)
            {
                co_await Windows::System::Launcher::LaunchUriAsync(Windows::Foundation::Uri(uri));
            }
            else
            {
				co_await Windows::System::Launcher::LaunchUriAsync(uri);
			}
        }

        HWND GetWindowHandle();
        Microsoft::UI::Windowing::AppWindow GetAppWindow();
    };
}

namespace winrt::MainApplication::factory_implementation
{
    struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow>
    {
    };
}
