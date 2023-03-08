// Author: Lucas Oliveira Vilas-B�as
// Year: 2023
// Repository: https://github.com/lucoiso/PasswordManagement

#include "pch.h"

#include "ApplicationSettings.xaml.h"
#include "ApplicationSettings.g.cpp"

#include "App.xaml.h"
#include "DialogHelper.h"
#include "SettingsHelper.h"

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MainApplication::implementation
{
    ApplicationSettings::ApplicationSettings()
    {
        InitializeComponent();

        Style(Application::Current().Resources().Lookup(box_value(L"DefaultContentDialogStyle")).as<Microsoft::UI::Xaml::Style>());
        DefaultButton(Microsoft::UI::Xaml::Controls::ContentDialogButton::Primary);

        PrimaryButtonClick(
            [this]([[maybe_unused]] auto&&, [[maybe_unused]] auto&&)
            {
                SaveSettings();
                Hide();
                App::RestartApplication();
            }
        );

        SecondaryButtonClick(
            [this]([[maybe_unused]] auto&&, [[maybe_unused]] auto&&)
            {
                SaveSettings();
                Hide();
            }
        );
    }

    void ApplicationSettings::Panel_Loaded([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] Microsoft::UI::Xaml::RoutedEventArgs const& args)
    {
        LUPASS_LOG_FUNCTION();

        InitializeSettingsProperties();
    }

    Windows::Foundation::IAsyncAction ApplicationSettings::BP_Reset_Click([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] Microsoft::UI::Xaml::RoutedEventArgs const& args)
    {
        Hide();
        auto confirm_dialog = Helper::CreateContentDialog(XamlRoot(), L"Reset to Defaults", L"This operation will reset all settings to default. It will not delete login data, only application settings. Proceed?", true, true);

        switch ((co_await confirm_dialog.ShowAsync()))
        {
            case Microsoft::UI::Xaml::Controls::ContentDialogResult::Primary:
            {
                confirm_dialog.Hide();
                Helper::SetSettingsToDefault();
                App::RestartApplication();
            }
        }
    }

    Windows::Foundation::IAsyncAction ApplicationSettings::InitializeSettingsProperties()
    {
        LUPASS_LOG_FUNCTION();

        if (!(co_await Windows::Security::Credentials::KeyCredentialManager::IsSupportedAsync()))
        {
            TS_WindowsHello().IsOn(false);
            TS_WindowsHello().IsEnabled(false);
        }
        else
        {
            TS_WindowsHello().IsOn(Helper::GetSettingValue<bool>(SETTING_ENABLE_WINDOWS_HELLO));
        }

        TS_SystemTray().IsOn(Helper::GetSettingValue<bool>(SETTING_ENABLE_SYSTEM_TRAY));
        TS_Shortcuts().IsOn(Helper::GetSettingValue<bool>(SETTING_ENABLE_SHORTCUTS));
    }

    void ApplicationSettings::SaveSettings()
    {
        LUPASS_LOG_FUNCTION();

        Helper::InsertSettingValue(SETTING_ENABLE_WINDOWS_HELLO, TS_WindowsHello().IsOn());
		Helper::InsertSettingValue(SETTING_ENABLE_SYSTEM_TRAY, TS_SystemTray().IsOn());
        Helper::InsertSettingValue(SETTING_ENABLE_SHORTCUTS, TS_Shortcuts().IsOn());
    }
}
