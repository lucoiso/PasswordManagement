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

        auto app_window = GetAppWindow();
        app_window.SetIcon(ICON_NAME);
        app_window.Title(APP_NAME);

        try
        {
            app_window.TitleBar().BackgroundColor(Windows::UI::Colors::Black());
            app_window.TitleBar().ForegroundColor(Windows::UI::Colors::White());
            app_window.TitleBar().ButtonBackgroundColor(Windows::UI::Colors::Black());
            app_window.TitleBar().ButtonForegroundColor(Windows::UI::Colors::White());
        }
        catch (const hresult_error& e)
        {
            Helper::PrintDebugLine(e.message());
        }

        app_window.Closing([this, app_window]([[maybe_unused]] const auto& sender, const auto& args)
        { 
            if (Helper::GetSettingValue<bool>(SETTING_ENABLE_SYSTEM_TRAY))
            {
                args.Cancel(true);
                app_window.Hide();
            }
        });
    }

    Windows::Foundation::IAsyncAction MainApplication::implementation::MainWindow::FR_MainFrame_Loaded([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] RoutedEventArgs const& args)
    {
        LUPASS_LOG_FUNCTION();

        co_await InitializeLicenseInformation();

        const bool has_license = co_await Helper::HasLicenseActive();
        if (!has_license)
        {
            const auto dialog_result = co_await Helper::CreateContentDialog(Content().XamlRoot(), L"Could not find an active license", L"You currently do not have a valid license. The application will limit its functionalities to only viewing and exporting.", true, true).ShowAsync();
            if (dialog_result == Microsoft::UI::Xaml::Controls::ContentDialogResult::Primary)
            {
                co_await LaunchUri(to_hstring(APP_SUBSCRIPTION_URI));
			}
        }

        FR_MainFrame().Navigate(xaml_typename<MainApplication::MainPage>(), box_value(has_license));
    }

    Windows::Foundation::IAsyncAction MainWindow::InitializeLicenseInformation()
    {
        LUPASS_LOG_FUNCTION();

        HB_LicenseData().NavigateUri(Windows::Foundation::Uri(to_hstring(APP_SUBSCRIPTION_URI)));

        if constexpr (ENABLE_DEBBUGGING)
        {
            HB_LicenseData().Content(box_value(L"License Status: Debug"));
            co_return;
        }

        const auto current_license_information = co_await Helper::GetAddonSubscriptionInfo();
        
        if (!current_license_information)
        {
            HB_LicenseData().Content(box_value(L"License Status: Invalid"));
            co_return;
        }

        std::string mode = "License Status: ";
        if (current_license_information.IsActive())
        {
            mode += "Active";
        }
        else
        {
            mode += "Inactive";
        }

        const auto formatter = Windows::Globalization::DateTimeFormatting::DateTimeFormatter::ShortDate();
        const std::string expiration = "Expiration Date: " + to_string(formatter.Format(current_license_information.ExpirationDate()));

        const hstring license_information = to_hstring(mode + ". " + expiration);
        Helper::PrintDebugLine(license_information);
        HB_LicenseData().Content(box_value(license_information));
    }

    void MainWindow::BT_Settings_Clicked([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] Microsoft::UI::Xaml::RoutedEventArgs const& args)
    {
        LUPASS_LOG_FUNCTION();

        Helper::InvokeSettingsDialog(Content().XamlRoot());
    }

    Windows::Foundation::IAsyncAction MainWindow::HB_LicenseData_Click(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args)
    {
        LUPASS_LOG_FUNCTION();

        co_await LaunchUri(HB_LicenseData().NavigateUri());
    }
    
    HWND MainWindow::GetWindowHandle()
    {
        LUPASS_LOG_FUNCTION();

        const auto windowNative{ this->try_as<IWindowNative>() };
        check_bool(windowNative);

        HWND output{ 0 };
        windowNative->get_WindowHandle(&output);

        return output;
    }
    
    Microsoft::UI::Windowing::AppWindow MainWindow::GetAppWindow()
    {
        LUPASS_LOG_FUNCTION();

        const Microsoft::UI::WindowId CurrentWindowID = Microsoft::UI::GetWindowIdFromWindow(GetWindowHandle());
        Microsoft::UI::Windowing::AppWindow CurrentAppWindow = Microsoft::UI::Windowing::AppWindow::GetFromWindowId(CurrentWindowID);

        return CurrentAppWindow;
    }
}
