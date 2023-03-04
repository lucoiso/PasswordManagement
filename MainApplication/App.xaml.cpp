// Author: Lucas Oliveira Vilas-Bôas
// Year: 2022
// Repository: https://github.com/lucoiso/PasswordManagement

#include "pch.h"

#include "App.xaml.h"
#include "MainWindow.xaml.h"

#include <windows.h>
#include <windowsx.h>
#include <shellapi.h>

// Defina uma mensagem personalizada para o menu de contexto
#define WM_TRAYICON (WM_USER + 1)

// Declarar um identificador exclusivo para o ícone da bandeja do sistema
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
    if (const auto local_settings = Windows::Storage::ApplicationData::Current().LocalSettings(); local_settings.Values().HasKey(L"password_set"))
    {
        local_settings.Values().Remove(L"password_set");
    }

    RemoveTrayIcon();
}

void App::OnLaunched(LaunchActivatedEventArgs const&)
{
    m_window = make<MainWindow>();
    m_window.Activate();

    if (!AddTrayIcon())
    {
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
        case WM_TRAYICON:
        {
            switch (lParam)
            {
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
                            Application::Current().Exit();
                            break;
                    }

                    break;
                }

                default: break;
            }
        }

        default: return DefWindowProc(hwnd, uMsg, wParam, lParam);;
    }    
}

BOOL App::InitNotifyIconData()
{
    WNDCLASSEX wc = { 0 };
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = TrayIconCallback;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = L"TrayIconCallback";
    RegisterClassEx(&wc);

    m_tray_hwnd = CreateWindowEx(0, wc.lpszClassName, wc.lpszClassName, WS_OVERLAPPEDWINDOW, 0, 0, 0, 0, NULL, NULL, wc.hInstance, NULL);

    NOTIFYICONDATA notify_icon_data;
    notify_icon_data.cbSize = sizeof(NOTIFYICONDATA);
    notify_icon_data.hWnd = m_tray_hwnd;
    notify_icon_data.uID = ID_TRAYICON;
    notify_icon_data.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    notify_icon_data.uCallbackMessage = WM_TRAYICON;

    HICON hIcon = NULL;
    ExtractIconEx(L"Icon.ico", 0, NULL, &hIcon, 1);
    notify_icon_data.hIcon = hIcon;
    wcscpy_s(notify_icon_data.szTip, L"Password Manager");

    Shell_NotifyIcon(NIM_ADD, &notify_icon_data);

    auto dispatcher_queue = Microsoft::UI::Dispatching::DispatcherQueue::GetForCurrentThread();
    auto Timer = dispatcher_queue.CreateTimer();
    Timer.Interval(TimeSpan(std::chrono::seconds(1)));
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