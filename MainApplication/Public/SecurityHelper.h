// Author: Lucas Oliveira Vilas-Bôas
// Year: 2023
// Repository: https://github.com/lucoiso/PasswordManagement

#pragma once

#include "pch.h"

#include "DialogHelper.h"
#include "SettingsHelper.h"

#include <Constants.h>

using namespace winrt::MainApplication;

namespace winrt::Helper
{
    inline Windows::Foundation::IAsyncOperation<bool> RequestUserCredentials(const Microsoft::UI::Xaml::XamlRoot& root)
    {
        if (!Helper::GetSettingValue<bool>(SETTING_ENABLE_WINDOWS_HELLO))
        {
            co_return true;
        }

        if (!(co_await Windows::Security::Credentials::KeyCredentialManager::IsSupportedAsync()))
        {
            if (!Helper::HasSettingKey(INVALID_SECURITY_ENVIRONMENT_ID))
            {
                co_await CreateContentDialog(root, L"Error", L"Windows Hello! wasn't configured in the current platform. Consider enabling this functionality to improve security.", false, true).ShowAsync();
                Helper::InsertSettingValue(INVALID_SECURITY_ENVIRONMENT_ID, true);
            }

            co_return true;
        }

        if (Helper::HasSettingKey(SECURITY_KEY_SET_ID))
        {
            co_return true;
        }

        const auto username = unbox_value<hstring>(co_await Windows::System::User::GetDefault().GetPropertyAsync(Windows::System::KnownUserProperties::AccountName()));
        const auto credentials_request = co_await Windows::Security::Credentials::KeyCredentialManager::RequestCreateAsync(username, Windows::Security::Credentials::KeyCredentialCreationOption::ReplaceExisting);

        switch (credentials_request.Status())
        {
            case Windows::Security::Credentials::KeyCredentialStatus::Success:
                Helper::InsertSettingValue(SECURITY_KEY_SET_ID, true);
                co_return true;

            default: break;
        }

        co_return false;
    }
}