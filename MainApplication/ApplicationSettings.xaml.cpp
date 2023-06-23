// Author: Lucas Oliveira Vilas-Bôas
// Year: 2023
// Repository: https://github.com/lucoiso/PasswordManagement

#include "pch.h"

#include "ApplicationSettings.xaml.h"
#include "ApplicationSettings.g.cpp"

#include "App.xaml.h"

#include "DialogManager.h"

#include "Helpers/SettingsHelper.h"
#include "Helpers/SecurityHelper.h"

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
            [this]([[maybe_unused]] auto&&, [[maybe_unused]] auto&&) -> Windows::Foundation::IAsyncAction
            {
                if (co_await SaveSettings())
                {
                    Hide();
                    App::RestartApplication();
                }
            }
        );

        SecondaryButtonClick(
            [this]([[maybe_unused]] auto&&, [[maybe_unused]] auto&&) -> Windows::Foundation::IAsyncAction
            {
                if (co_await SaveSettings())
                {
                    Hide();
                }
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
        const auto result = co_await DialogManager::GetInstance().ShowDialogAsync(L"Reset to Defaults", L"This operation will reset all settings to default. It will not delete login data, only application settings. Proceed?", true, true);

        switch (result)
        {
            case Microsoft::UI::Xaml::Controls::ContentDialogResult::Primary:
            {
                if (!(co_await Helper::RequestUserCredentials()))
                {
                    co_return;
                }

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

    Windows::Foundation::IAsyncOperation<bool> ApplicationSettings::SaveSettings()
    {
        LUPASS_LOG_FUNCTION();

        if (!(co_await Helper::RequestUserCredentials()))
        {
            co_return false;
        }

        Helper::InsertSettingValue(SETTING_ENABLE_WINDOWS_HELLO, TS_WindowsHello().IsOn());
		Helper::InsertSettingValue(SETTING_ENABLE_SYSTEM_TRAY, TS_SystemTray().IsOn());
        Helper::InsertSettingValue(SETTING_ENABLE_SHORTCUTS, TS_Shortcuts().IsOn());

        co_return true;
    }
}