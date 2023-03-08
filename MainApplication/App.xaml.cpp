// Author: Lucas Oliveira Vilas-Bôas
// Year: 2023
// Repository: https://github.com/lucoiso/PasswordManagement

#include "pch.h"

#include "App.xaml.h"
#include "MainWindow.xaml.h"

#include <windows.h>
#include <windowsx.h>
#include <shellapi.h>

#include "SettingsHelper.h"
#include "DialogHelper.h"

constexpr auto WM_TRAYICON = (WM_USER + 1);
constexpr auto WM_TOGGLE_WINDOW = (WM_USER + 2);
constexpr auto WM_TOGGLE_GENERATOR = (WM_USER + 3);
constexpr auto WM_TOGGLE_SETTINGS = (WM_USER + 4);
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
    UnhandledException(
        [this](IInspectable const&, UnhandledExceptionEventArgs const& e)
        {
            if (IsDebuggerPresent())
            {
                auto errorMessage = e.Message();
                __debugbreak();
            }
        }
    );
#endif
}

App::~App()
{
    Helper::ClearSecurityIds();

    RemoveTrayIcon();
    UnregisterKeyboardShortcuts();
    UnregisterMouseHook();
}

Windows::Foundation::IAsyncAction App::OnLaunched([[maybe_unused]] LaunchActivatedEventArgs const& args)
{
    LUPASS_LOG_FUNCTION();

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
    LUPASS_LOG_FUNCTION();

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

void App::ToggleWindow(HWND hwnd, const bool can_hide)
{
    LUPASS_LOG_FUNCTION();

    try
    {
        auto application = Application::Current().as<App>();
        if (!application->Window().Visible())
        {
            application->Window().Activate();

            if (GetForegroundWindow() != hwnd)
            {
                SetForegroundWindow(hwnd);
            }
        }
        else if (can_hide)
        {
            application->Window().as<MainWindow>()->GetAppWindow().Hide();
        }
    }
    catch (const hresult_error& e)
    {
        Helper::PrintDebugLine(e.message());
    }
}

void App::RestartApplication()
{
    LUPASS_LOG_FUNCTION();

    try
    {
        Microsoft::Windows::AppLifecycle::AppInstance::GetCurrent().Restart(L"DESIRED_RESTART");
    }
    catch (const hresult_error& e)
    {
        Helper::PrintDebugLine(e.message());
    }
}

void App::CloseApplication()
{
    LUPASS_LOG_FUNCTION();

    try
    {
        Application::Current().Exit();
    }
    catch (const hresult_error& e)
    {
        Helper::PrintDebugLine(e.message());
    }
}

bool App::CheckSingleInstance(const Microsoft::Windows::AppLifecycle::AppInstance& instance)
{
    LUPASS_LOG_FUNCTION();

    if (!instance.IsCurrent())
    {
        SetForegroundWindow(FindWindow(TRAYICON_CLASSNAME, TRAYICON_CLASSNAME));

        try
        {
            Exit();
        }
        catch (const hresult_error& e)
        {
            Helper::PrintDebugLine(e.message());
        }

        return false;
    }

    return true;
}

void ProcessTrayIconMessage(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    static HMENU menu{ nullptr };

    const auto remove_menu = [&hwnd](HMENU& menu)
    {
        Application::Current().as<App>()->UnregisterMouseHook();

        if (!menu)
        {
            return;
        }

        POINT pt;
        if (!GetCursorPos(&pt))
        {
            return;
        }

        const int menu_item_count = GetMenuItemCount(menu);
        bool is_in_menu = false;

        for (int i = 0; i < menu_item_count; ++i)
        {
            RECT rc;
            if (GetMenuItemRect(hwnd, menu, i, &rc))
            {
                if (pt.x >= rc.left && pt.x <= rc.right && pt.y >= rc.top && pt.y <= rc.bottom)
                {
                    is_in_menu = true;
                    break;
                }
            }
        }

        if (!is_in_menu)
        {
            DestroyMenu(menu);
            EndMenu();

            menu = nullptr;
        }
    };

    switch (lParam)
    {
        case WM_LBUTTONUP:
        {
            static DWORD64 last_click_time;
            DWORD64 current_tick_time = GetTickCount64();

            if (current_tick_time - last_click_time < 500)
            {
                SendMessageW(hwnd, WM_TOGGLE_WINDOW, 0, 0);
            }

            last_click_time = current_tick_time;

            break;
        }

        case WM_RBUTTONDOWN:
        {
            menu = CreatePopupMenu();

            AppendMenuW(menu, MF_STRING, ID_TOGGLE_APPLICATION_VISIBILITY, Application::Current().as<App>()->Window().Visible() ? L"Hide Application" : L"Show Application");
            AppendMenuW(menu, MF_STRING, ID_TOGGLE_GENERATOR_VISIBILITY, L"Open Generator");
            AppendMenuW(menu, MF_STRING, ID_TOGGLE_SETTINGS_VISIBILITY, L"Open Settings");
            AppendMenuW(menu, MF_STRING, ID_CLOSE_APPLICATION, L"Exit Application");

            POINT cursor_position;
            GetCursorPos(&cursor_position);

            Application::Current().as<App>()->RegisterMouseHook();

            const UINT menu_flags = TPM_RETURNCMD | TPM_NONOTIFY | TPM_RIGHTBUTTON | TPM_LEFTBUTTON;
            const BOOL menu_item = TrackPopupMenuEx(menu, menu_flags, cursor_position.x, cursor_position.y, hwnd, nullptr);

            switch (menu_item)
            {
                case ID_TOGGLE_APPLICATION_VISIBILITY:
                    SendMessageW(hwnd, WM_TOGGLE_WINDOW, 0, 0);
                    break;

                case ID_TOGGLE_GENERATOR_VISIBILITY:
                    SendMessageW(hwnd, WM_SETFOCUS, 0, 0);
                    SendMessageW(hwnd, WM_TOGGLE_GENERATOR, 0, 0);
                    break;

                case ID_TOGGLE_SETTINGS_VISIBILITY:
                    SendMessageW(hwnd, WM_SETFOCUS, 0, 0);
                    SendMessageW(hwnd, WM_TOGGLE_SETTINGS, 0, 0);
                    break;

                case ID_CLOSE_APPLICATION:
                    SendMessageW(hwnd, WM_EXIT_APPLICATION, 0, 0);
                    break;

                default: break;
            }

            break;
        }

        case WM_LBUTTONDOWN:
            remove_menu(menu);
            break;

        default: break;
    }

    switch (wParam)
    {
        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
            remove_menu(menu);
            break;
    }
}

void ProcessHotKey(HWND hwnd, WPARAM wParam)
{
    switch (wParam)
    {
        case ID_APPLICATIONWINDOW_SHORTCUT:
            SendMessageW(hwnd, WM_TOGGLE_WINDOW, 0, 0);
            break;

        case ID_GENERATORWINDOW_SHORTCUT:
            SendMessageW(hwnd, WM_TOGGLE_GENERATOR, 0, 0);
            break;

        default: break;
    }
}

LRESULT CALLBACK App::ApplicationProcedure(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_SETFOCUS:
            App::ToggleWindow(hwnd, false);
            break;

        case WM_TOGGLE_WINDOW:
            App::ToggleWindow(hwnd);
            break;

        case WM_TOGGLE_GENERATOR:
            Helper::InvokeGeneratorDialog(Application::Current().as<App>()->Window().Content().XamlRoot());
            break;

        case WM_TOGGLE_SETTINGS:
            Helper::InvokeSettingsDialog(Application::Current().as<App>()->Window().Content().XamlRoot());
            break;

        case WM_EXIT_APPLICATION:
            App::CloseApplication();
            break;

        case WM_TRAYICON:
            ProcessTrayIconMessage(hwnd, wParam, lParam);
            break;

        case WM_HOTKEY:
            ProcessHotKey(hwnd, wParam);
            break;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

LRESULT App::LowLevelMouseProcedure(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode == HC_ACTION && (wParam == WM_LBUTTONDOWN || wParam == WM_RBUTTONDOWN))
    {
        SendMessageW(FindWindow(TRAYICON_CLASSNAME, TRAYICON_CLASSNAME), WM_TRAYICON, wParam, lParam);
    }

    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

void App::AddTrayIcon()
{
    LUPASS_LOG_FUNCTION();

    if (!Helper::GetSettingValue<bool>(SETTING_ENABLE_SYSTEM_TRAY))
    {
        return;
    }

    WNDCLASSEX wc = { 0 };
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = ApplicationProcedure;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.lpszClassName = TRAYICON_CLASSNAME;
    RegisterClassEx(&wc);

    m_tray_hwnd = CreateWindowEx(0, TRAYICON_CLASSNAME, TRAYICON_CLASSNAME, WS_POPUP, 0, 0, 0, 0, nullptr, nullptr, wc.hInstance, nullptr);

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
    LUPASS_LOG_FUNCTION();

    DestroyIcon(m_notify_icon_data.hIcon);
    Shell_NotifyIcon(NIM_DELETE, &m_notify_icon_data);
}

void App::RegisterKeyboardShortcuts()
{
    LUPASS_LOG_FUNCTION();

    if (!Helper::GetSettingValue<bool>(SETTING_ENABLE_SHORTCUTS))
    {
        return;
    }

    RegisterHotKey(m_tray_hwnd, ID_APPLICATIONWINDOW_SHORTCUT, MOD_CONTROL | MOD_ALT, 0x4E);
    RegisterHotKey(m_tray_hwnd, ID_GENERATORWINDOW_SHORTCUT, MOD_CONTROL | MOD_ALT, 0x4D);
}

void App::UnregisterKeyboardShortcuts()
{
    LUPASS_LOG_FUNCTION();

    UnregisterHotKey(m_tray_hwnd, ID_APPLICATIONWINDOW_SHORTCUT);
    UnregisterHotKey(m_tray_hwnd, ID_GENERATORWINDOW_SHORTCUT);
}

void App::RegisterMouseHook()
{
    m_mouse_hook = SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProcedure, nullptr, 0);
}

void App::UnregisterMouseHook()
{
    UnhookWindowsHookEx(m_mouse_hook);
}
