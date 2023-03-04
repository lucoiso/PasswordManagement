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
        
        void FR_MainFrame_Loaded(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args);

        Windows::Foundation::IAsyncAction InitializeContentAsync();

        HWND GetWindowHandle();
        
    private:
        Microsoft::UI::Windowing::AppWindow GetAppWindow();
        Microsoft::UI::Windowing::AppWindow m_app_window{ nullptr };
    };
}

namespace winrt::MainApplication::factory_implementation
{
    struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow>
    {
    };
}
