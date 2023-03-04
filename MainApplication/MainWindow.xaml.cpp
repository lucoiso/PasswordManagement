// Author: Lucas Oliveira Vilas-Bôas
// Year: 2023
// Repository: https://github.com/lucoiso/PasswordManagement

#include "pch.h"

#include "MainWindow.xaml.h"
#include "MainWindow.g.cpp"

#include "MainPage.xaml.h"
#include "App.xaml.h"

#include "DialogHelper.h"

#include <Constants.h>

#include <Microsoft.UI.Xaml.Window.h>
#include <winrt/Microsoft.UI.Interop.h>

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MainApplication::implementation
{
    MainWindow::MainWindow()
    {
        InitializeComponent();

        m_app_window = GetAppWindow();
        m_app_window.SetIcon(ICON_NAME);
        m_app_window.Title(APP_NAME);
		m_app_window.TitleBar().BackgroundColor(Windows::UI::Colors::Black());
        m_app_window.TitleBar().ForegroundColor(Windows::UI::Colors::White());
        m_app_window.TitleBar().ButtonBackgroundColor(Windows::UI::Colors::Black());
        m_app_window.TitleBar().ButtonForegroundColor(Windows::UI::Colors::White());

        m_app_window.Closing([this]([[maybe_unused]] const auto& sender, const auto& args) { 
            args.Cancel(true);
            m_app_window.Hide();
        });
    }

    void MainApplication::implementation::MainWindow::FR_MainFrame_Loaded([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] RoutedEventArgs const& args)
    {
        Controls::ProgressBar progress_bar;
        progress_bar.IsIndeterminate(true);
        const auto loading_dialog = Helper::CreateContentDialog(Content().XamlRoot(), L"Loading...", progress_bar, false, false);

        loading_dialog.ShowAsync();
        InitializeContentAsync();
        loading_dialog.Hide();
    }

    Windows::Foundation::IAsyncAction MainWindow::InitializeContentAsync()
    {
        FR_MainFrame().Navigate(xaml_typename<MainApplication::MainPage>());
        co_return;
    }
    
    HWND MainWindow::GetWindowHandle()
    {
        const auto windowNative{ this->try_as<IWindowNative>() };
        check_bool(windowNative);

        HWND output{ 0 };
        windowNative->get_WindowHandle(&output);

        return output;
    }
    
    Microsoft::UI::Windowing::AppWindow MainWindow::GetAppWindow()
    {
        const Microsoft::UI::WindowId CurrentWindowID = Microsoft::UI::GetWindowIdFromWindow(GetWindowHandle());
        Microsoft::UI::Windowing::AppWindow CurrentAppWindow = Microsoft::UI::Windowing::AppWindow::GetFromWindowId(CurrentWindowID);

        return CurrentAppWindow;
    }
}
