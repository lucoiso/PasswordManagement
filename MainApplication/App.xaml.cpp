// Author: Lucas Oliveira Vilas-Bôas
// Year: 2023
// Repository: https://github.com/lucoiso/PasswordManagement

#include "pch.h"

#include "App.xaml.h"
#include "MainWindow.xaml.h"

#include <windows.h>
#include <windowsx.h>
#include <shellapi.h>

#include <Helper.h>
#include <Constants.h>

#include "SettingsHelper.h"
#include "DialogHelper.h"

constexpr auto WM_TRAYICON = (WM_USER + 1);
constexpr auto WM_ACTIVATE_WINDOW = (WM_USER + 2);
constexpr auto WM_ACTIVATE_GENERATOR = (WM_USER + 3);
constexpr auto WM_ACTIVATE_SETTINGS = (WM_USER + 4);
constexpr auto WM_EXIT_APPLICATION = (WM_USER + 5);

constexpr auto ID_TRAYICON = (WM_USER + 6);

constexpr auto ID_APPLICATIONWINDOW_SHORTCUT = (WM_USER + 7);
constexpr auto ID_GENERATORWINDOW_SHORTCUT = (WM_USER + 8);

constexpr auto ID_TOGGLE_APPLICATION_VISIBILITY = (WM_USER + 9);
constexpr auto ID_TOGGLE_GENERATOR_VISIBILITY = (WM_USER + 10);
constexpr auto ID_TOGGLE_SETTINGS_VISIBILITY = (WM_USER + 11);
constexpr auto ID_CLOSE_APPLICATION = (WM_USER + 12);

using namespace winrt;
using namespace Windows::Foundation;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Controls;
using namespace Microsoft::UI::Xaml::Navigation;
using namespace MainApplication;
using namespace MainApplication::implementation;

App::App()
{
    InitializeComponent();

#if defined _DEBUG && !defined DISABLE_XAML_GENERATED_BREAK_ON_UNHANDLED_EXCEPTION
    UnhandledException([this](IInspectable const&, UnhandledExceptionEventArgs const& e)
    {
        if (IsDebuggerPresent())
        {
            auto errorMessage = e.Message();
            __debugbreak();
        }
    });
#endif
}

App::~App()
{
    Helper::ClearSecurityIds();

    RemoveTrayIcon();
    UnregisterKeyboardShortcuts();
}

Windows::Foundation::IAsyncAction App::OnLaunched([[maybe_unused]] LaunchActivatedEventArgs const& args)
{
    const auto app_instance = Microsoft::Windows::AppLifecycle::AppInstance::FindOrRegisterForKey(APP_INSTANCE_KEY);

    if (!CheckSingleInstance(app_instance))
    {
        co_return;
    }

    co_await Helper::InitializeSettings();

    AddTrayIcon();
    RegisterKeyboardShortcuts();
    InitializeWindow(app_instance.GetActivatedEventArgs().Kind());
}

void winrt::MainApplication::implementation::App::InitializeWindow(const Microsoft::Windows::AppLifecycle::ExtendedActivationKind& activation_kind)
{
    m_window = make<MainWindow>();

    if (activation_kind != Microsoft::Windows::AppLifecycle::ExtendedActivationKind::StartupTask)
    {
        m_window.Activate();
    }
}

Window MainApplication::implementation::App::Window() const
{
    return m_window;
}

HWND MainApplication::implementation::App::GetCurrentWindowHandle()
{
    return Application::Current().as<App>()->Window().as<MainWindow>()->GetWindowHandle();
}

void App::ActivateWindow()
{
    try
    {
        if (!Application::Current().as<App>()->Window().Visible())
        {
            Application::Current().as<App>()->Window().Activate();
        }
    }
    catch (...)
    {
    }
}

void App::RestartApplication()
{
    try
    {
        Microsoft::Windows::AppLifecycle::AppInstance::GetCurrent().Restart(L"DESIRED_RESTART");
    }
    catch (...)
    {
    }
}

void App::CloseApplication()
{
    try
    {
        Application::Current().Exit();
    }
    catch (...)
    {
    }
}

bool App::CheckSingleInstance(const Microsoft::Windows::AppLifecycle::AppInstance& instance)
{
    if (!instance.IsCurrent())
    {
        SendMessage(FindWindow(TRAYICON_CLASSNAME, TRAYICON_CLASSNAME), WM_SHOWWINDOW, 0, 0);
        
        try
        {
            Exit();
        }
        catch (...)
        {
        }

        return false;
    }

    return true;
}

void ProcessTrayIconMessage(HWND hwnd, LPARAM lParam)
{
    switch (lParam)
    {
        case WM_LBUTTONUP:
        {
            static DWORD64 last_click_time;
            DWORD64 current_tick_time = GetTickCount64();

            if (current_tick_time - last_click_time < 1000)
            {
                SendMessage(hwnd, WM_ACTIVATE_WINDOW, 0, 0);
            }

            last_click_time = current_tick_time;

            break;
        }

        case WM_RBUTTONDOWN:
        {
            HMENU hMenu = CreatePopupMenu();

            AppendMenu(hMenu, MF_STRING, ID_TOGGLE_APPLICATION_VISIBILITY, Application::Current().as<App>()->Window().Visible() ? L"Minimize" : L"Show");
            // AppendMenu(hMenu, MF_STRING, ID_TOGGLE_GENERATOR_VISIBILITY, L"Open Generator");
            // AppendMenu(hMenu, MF_STRING, ID_TOGGLE_SETTINGS_VISIBILITY, L"Settings");
            AppendMenu(hMenu, MF_STRING, ID_CLOSE_APPLICATION, L"Exit");

            POINT cursorPos;
            GetCursorPos(&cursorPos);

            const int menuItem = TrackPopupMenu(hMenu, TPM_RETURNCMD | TPM_NONOTIFY, cursorPos.x, cursorPos.y, 0, hwnd, nullptr);

            DestroyMenu(hMenu);

            switch (menuItem)
            {
                case ID_TOGGLE_APPLICATION_VISIBILITY:
                    SendMessage(hwnd, WM_ACTIVATE_WINDOW, 0, 0);
                    break;

                case ID_TOGGLE_GENERATOR_VISIBILITY:
                    SendMessage(hwnd, WM_ACTIVATE_GENERATOR, 0, 0);
                    break;

                case ID_TOGGLE_SETTINGS_VISIBILITY:
                    SendMessage(hwnd, WM_ACTIVATE_SETTINGS, 0, 0);
                    break;

                case ID_CLOSE_APPLICATION:
                    SendMessage(hwnd, WM_EXIT_APPLICATION, 0, 0);
                    break;

                default: break;
            }

            break;
        }

        default: break;
    }
}

void ProcessHotKey(HWND hwnd, WPARAM wParam)
{
    switch (wParam)
    {
        case ID_APPLICATIONWINDOW_SHORTCUT:
            SendMessage(hwnd, WM_ACTIVATE_WINDOW, 0, 0);
            break;

        case ID_GENERATORWINDOW_SHORTCUT:
            SendMessage(hwnd, WM_ACTIVATE_GENERATOR, 0, 0);
            break;

        default: break;
    }
}

LRESULT CALLBACK App::TrayIconCallback(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_ACTIVATE_WINDOW:
            App::ActivateWindow();
            break;

        case WM_ACTIVATE_GENERATOR:
            Helper::InvokeGeneratorDialog(Application::Current().as<App>()->Window().Content().XamlRoot());
            break;

        case WM_ACTIVATE_SETTINGS:
            Helper::InvokeSettingsDialog(Application::Current().as<App>()->Window().Content().XamlRoot());
            break;

        case WM_EXIT_APPLICATION:
            App::CloseApplication();
            break;

        case WM_TRAYICON:
            ProcessTrayIconMessage(hwnd, lParam);
            break;

        case WM_HOTKEY:
            ProcessHotKey(hwnd, wParam);
            break;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void App::AddTrayIcon()
{
    if (!Helper::GetSettingValue<bool>(SETTING_ENABLE_SYSTEM_TRAY))
    {
        return;
    }

    WNDCLASSEX wc = { 0 };
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = TrayIconCallback;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.lpszClassName = TRAYICON_CLASSNAME;
    RegisterClassEx(&wc);

    m_tray_hwnd = CreateWindowEx(0, TRAYICON_CLASSNAME, TRAYICON_CLASSNAME, WS_OVERLAPPEDWINDOW, 0, 0, 0, 0, nullptr, nullptr, wc.hInstance, nullptr);

    m_notify_icon_data.cbSize = sizeof(NOTIFYICONDATA);
    m_notify_icon_data.hWnd = m_tray_hwnd;
    m_notify_icon_data.uID = ID_TRAYICON;
    m_notify_icon_data.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    m_notify_icon_data.uCallbackMessage = WM_TRAYICON;

    HICON hIcon = nullptr;
    ExtractIconEx(ICON_NAME, 0, nullptr, &hIcon, 1);
    m_notify_icon_data.hIcon = hIcon;
    wcscpy_s(m_notify_icon_data.szTip, APP_NAME);

    Shell_NotifyIcon(NIM_ADD, &m_notify_icon_data);
}

void App::RemoveTrayIcon()
{
    DestroyIcon(m_notify_icon_data.hIcon);
    Shell_NotifyIcon(NIM_DELETE, &m_notify_icon_data);
}

void App::RegisterKeyboardShortcuts()
{
    if (!Helper::GetSettingValue<bool>(SETTING_ENABLE_SHORTCUTS))
    {
        return;
    }

    RegisterHotKey(m_tray_hwnd, ID_APPLICATIONWINDOW_SHORTCUT, MOD_CONTROL | MOD_ALT, 0x4E);
    RegisterHotKey(m_tray_hwnd, ID_GENERATORWINDOW_SHORTCUT, MOD_CONTROL | MOD_ALT, 0x4D);
}

void App::UnregisterKeyboardShortcuts()
{
    UnregisterHotKey(m_tray_hwnd, ID_APPLICATIONWINDOW_SHORTCUT);
    UnregisterHotKey(m_tray_hwnd, ID_GENERATORWINDOW_SHORTCUT);
}