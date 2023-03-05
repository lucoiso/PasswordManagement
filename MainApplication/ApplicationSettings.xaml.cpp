// Author: Lucas Oliveira Vilas-B�as
// Year: 2023
// Repository: https://github.com/lucoiso/PasswordManagement

#include "pch.h"

#include "ApplicationSettings.xaml.h"
#include "ApplicationSettings.g.cpp"

#include <Helper.h>

#include "App.xaml.h"
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

        PrimaryButtonClick([this]([[maybe_unused]] auto&&, [[maybe_unused]] auto&&)
        {
            SaveSettings();
            Hide();
            App::RestartApplication();
		});

        SecondaryButtonClick([this]([[maybe_unused]] auto&&, [[maybe_unused]] auto&&)
        {
            SaveSettings();
			Hide();
		});
    }

    void ApplicationSettings::Panel_Loaded([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] Microsoft::UI::Xaml::RoutedEventArgs const& args)
    {
        InitializeSettingsProperties();
    }

    Windows::Foundation::IAsyncAction ApplicationSettings::InitializeSettingsProperties()
    {
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
        Helper::InsertSettingValue(SETTING_ENABLE_WINDOWS_HELLO, TS_WindowsHello().IsOn());
		Helper::InsertSettingValue(SETTING_ENABLE_SYSTEM_TRAY, TS_SystemTray().IsOn());
        Helper::InsertSettingValue(SETTING_ENABLE_SHORTCUTS, TS_Shortcuts().IsOn());
    }
}
