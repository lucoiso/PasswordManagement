// Author: Lucas Oliveira Vilas-Bôas
// Year: 2022
// Repository: https://github.com/lucoiso/PasswordManagement

#pragma once

#include "App.xaml.g.h"

namespace winrt::MainApplication::implementation
{
    struct App : AppT<App>
    {
        App();
        ~App();

        void OnLaunched(Microsoft::UI::Xaml::LaunchActivatedEventArgs const&);

        Microsoft::UI::Xaml::Window Window() const;

        static HWND GetCurrentWindowHandle();

    private:
        Microsoft::UI::Xaml::Window m_window{ nullptr };

        HWND m_tray_hwnd;
        
        NOTIFYICONDATA nid = {};
        static LRESULT CALLBACK TrayIconCallback(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
        BOOL InitNotifyIconData();
        BOOL AddTrayIcon();
        BOOL RemoveTrayIcon();
    };
}
