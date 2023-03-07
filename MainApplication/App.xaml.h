// Author: Lucas Oliveira Vilas-Bôas
// Year: 2023
// Repository: https://github.com/lucoiso/PasswordManagement

#pragma once

#include "App.xaml.g.h"

namespace winrt::MainApplication::implementation
{
    struct App : AppT<App>
    {
        App();
        ~App();

        Windows::Foundation::IAsyncAction OnLaunched(Microsoft::UI::Xaml::LaunchActivatedEventArgs const& args);

        void InitializeWindow(const Microsoft::Windows::AppLifecycle::ExtendedActivationKind& activation_kind);

        Microsoft::UI::Xaml::Window Window() const;

        static HWND GetCurrentWindowHandle();

        static void ToggleWindow(HWND hwnd, const bool can_hide = true);

        static void RestartApplication();
        static void CloseApplication();

        bool CheckSingleInstance(const Microsoft::Windows::AppLifecycle::AppInstance& instance);

    private:
        Microsoft::UI::Xaml::Window m_window{ nullptr };
        Microsoft::UI::Dispatching::DispatcherQueueTimer m_timer{ nullptr };

        HWND m_tray_hwnd;
        HHOOK m_mouse_hook;
        
        NOTIFYICONDATA m_notify_icon_data = {};
        static LRESULT CALLBACK ApplicationProcedure(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
        static LRESULT CALLBACK LowLevelMouseProcedure(int nCode, WPARAM wParam, LPARAM lParam);

        void AddTrayIcon();
        void RemoveTrayIcon();

        void RegisterKeyboardShortcuts();
        void UnregisterKeyboardShortcuts();

        void RegisterMouseHook();
        void UnregisterMouseHook();
    };
}
