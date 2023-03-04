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

#define WM_TRAYICON (WM_USER + 1)
#define ID_TRAYICON 1

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

winrt::MainApplication::implementation::App::~App()
{
    if (const auto local_settings = Windows::Storage::ApplicationData::Current().LocalSettings(); local_settings.Values().HasKey(SECURITY_KEY_SET_ID))
    {
        local_settings.Values().Remove(SECURITY_KEY_SET_ID);
    }

    RemoveTrayIcon();
}

void App::OnLaunched(LaunchActivatedEventArgs const& args)
{    
    if (const auto app_instance = Microsoft::Windows::AppLifecycle::AppInstance::FindOrRegisterForKey(APP_INSTANCE_KEY); !app_instance.IsCurrent())
    {
        SendMessage(FindWindow(TRAYICON_CLASSNAME, TRAYICON_CLASSNAME), WM_SHOWWINDOW, FALSE, 0);
        Exit();
        return;
    }

    m_window = make<MainWindow>();

    if (!AddTrayIcon())
    {
        Helper::PrintDebugLine(L"Failed to add System Tray Icon");
    }

    if (args.UWPLaunchActivatedEventArgs().Kind() == Windows::ApplicationModel::Activation::ActivationKind::Launch)
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

LRESULT CALLBACK App::TrayIconCallback(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_SHOWWINDOW:
        {
            if (!Application::Current().as<App>()->Window().Visible())
            {
                Application::Current().as<App>()->Window().Activate();
            }
            break;
        }

        case WM_TRAYICON:
        {
            switch (lParam)
            {
                case WM_LBUTTONUP:
                {
                    static DWORD last_click_time;
                    DWORD current_tick_time = GetTickCount();

                    if (current_tick_time - last_click_time < 1000)
                    {
                        if (!Application::Current().as<App>()->Window().Visible())
                        {
                            Application::Current().as<App>()->Window().Activate();
                        }
                    }

                    last_click_time = current_tick_time;
                    break;
                }

                case WM_RBUTTONDOWN:
                {
                    HMENU hMenu = CreatePopupMenu();
                    AppendMenu(hMenu, MF_STRING, 1, L"Open");
                    AppendMenu(hMenu, MF_STRING, 2, L"Close");

                    POINT cursorPos;
                    GetCursorPos(&cursorPos);

                    int menuItem = TrackPopupMenu(hMenu, TPM_RETURNCMD | TPM_NONOTIFY, cursorPos.x, cursorPos.y, 0, hwnd, NULL);

                    DestroyMenu(hMenu);

                    switch (menuItem)
                    {
                        case 1:
                            if (!Application::Current().as<App>()->Window().Visible())
                            {
                                Application::Current().as<App>()->Window().Activate();
                            }

                            break;

                        case 2:
                            try
                            {
                                Application::Current().Exit();
                            }
                            catch (...)
                            {
                            }
                            break;

                        default: break;
                        }

                        break;
                }

                default: break;
            }
        }

        default: break;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

BOOL App::InitNotifyIconData()
{
    WNDCLASSEX wc = { 0 };
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = TrayIconCallback;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = TRAYICON_CLASSNAME;
    RegisterClassEx(&wc);

    m_tray_hwnd = CreateWindowEx(0, TRAYICON_CLASSNAME, TRAYICON_CLASSNAME, WS_OVERLAPPEDWINDOW, 0, 0, 0, 0, NULL, NULL, wc.hInstance, NULL);

    NOTIFYICONDATA notify_icon_data;
    notify_icon_data.cbSize = sizeof(NOTIFYICONDATA);
    notify_icon_data.hWnd = m_tray_hwnd;
    notify_icon_data.uID = ID_TRAYICON;
    notify_icon_data.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    notify_icon_data.uCallbackMessage = WM_TRAYICON;

    HICON hIcon = NULL;
    ExtractIconEx(ICON_NAME, 0, NULL, &hIcon, 1);
    notify_icon_data.hIcon = hIcon;
    wcscpy_s(notify_icon_data.szTip, APP_NAME);

    Shell_NotifyIcon(NIM_ADD, &notify_icon_data);

    auto dispatcher_queue = Microsoft::UI::Dispatching::DispatcherQueue::GetForCurrentThread();
    auto Timer = dispatcher_queue.CreateTimer();
    Timer.Interval(TimeSpan(std::chrono::milliseconds(500)));
    Timer.Tick([this](auto&&, auto&&)
    {
		MSG msg = { 0 };
        while (PeekMessage(&msg, m_tray_hwnd, 0, 0, PM_REMOVE))
        {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	});

    return 0;
}

BOOL App::AddTrayIcon()
{
    if (!InitNotifyIconData())
    {
        return FALSE;
    }

    return TRUE;
}

BOOL App::RemoveTrayIcon()
{
    DestroyIcon(nid.hIcon);

    if (!Shell_NotifyIcon(NIM_DELETE, &nid))
    {
        return FALSE;
    }

    return TRUE;
}

#undef WM_TRAYICON
#undef ID_TRAYICON