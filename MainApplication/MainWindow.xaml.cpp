// Author: Lucas Oliveira Vilas-Bôas
// Year: 2023
// Repository: https://github.com/lucoiso/PasswordManagement

#include "pch.h"

#include "MainWindow.xaml.h"
#include "MainWindow.g.cpp"

#include "MainPage.xaml.h"
#include "App.xaml.h"

#include "DialogHelper.h"
#include "SettingsHelper.h"
#include "SecurityHelper.h"

#include <Constants.h>
#include <iomanip>
#include <locale>
#include <codecvt>

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

        m_app_window.Closing([this]([[maybe_unused]] const auto& sender, const auto& args) 
        { 
            if (Helper::GetSettingValue<bool>(SETTING_ENABLE_SYSTEM_TRAY))
            {
                args.Cancel(true);
                m_app_window.Hide();
            }
        });
    }

    Windows::Foundation::IAsyncAction MainApplication::implementation::MainWindow::FR_MainFrame_Loaded([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] RoutedEventArgs const& args)
    {
        InitializeLicenseInformation();

        if (!Helper::HasLicenseActive())
        {
            co_await Helper::CreateContentDialog(Content().XamlRoot(), L"Your license has expired", L"The application will limit the features to export & view only.", false, true).ShowAsync();
        }

        FR_MainFrame().Navigate(xaml_typename<MainApplication::MainPage>());
    }

    void MainWindow::InitializeLicenseInformation()
    {
        auto current_license_information = Windows::ApplicationModel::Store::CurrentAppSimulator::LicenseInformation();
        
        std::string mode = "License Mode: ";
        if (current_license_information.IsTrial())
        {
            mode += "Trial";
        }
        else if (current_license_information.IsActive())
        {
            mode += "Full";
        }
        else
        {
            mode += "Invalid";
        }

        const auto formatter = Windows::Globalization::DateTimeFormatting::DateTimeFormatter::ShortDate();
        const std::string expiration = "Expiration Date: " + to_string(formatter.Format(current_license_information.ExpirationDate()));

        TB_LicenseData().Text(to_hstring(mode + ". " + expiration));
    }

    void MainWindow::BT_Settings_Clicked([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] Microsoft::UI::Xaml::RoutedEventArgs const& args)
    {
        Helper::InvokeSettingsDialog(Content().XamlRoot());
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
