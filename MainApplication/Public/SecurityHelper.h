// Author: Lucas Oliveira Vilas-Bôas
// Year: 2023
// Repository: https://github.com/lucoiso/PasswordManagement

#pragma once

#include "pch.h"

#include "DialogHelper.h"

#include <Constants.h>

using namespace winrt::MainApplication;

namespace winrt::Helper
{
	inline Windows::Foundation::IAsyncOperation<bool> RequestUserCredentials(const Microsoft::UI::Xaml::XamlRoot& root)
	{
        if (const auto local_settings = Windows::Storage::ApplicationData::Current().LocalSettings(); !local_settings.Values().HasKey(SECURITY_KEY_SET_ID))
        {
            // A senha ainda não foi inserida. Solicitar a senha do Credential Manager.
            if (!(co_await Windows::Security::Credentials::KeyCredentialManager::IsSupportedAsync()))
            {
                co_await CreateContentDialog(root, L"Error", L"Windows Hello! wasn't configured in the current platform.", false, true).ShowAsync();
                co_return false;
            }

            const auto username = unbox_value<hstring>(co_await Windows::System::User::GetDefault().GetPropertyAsync(Windows::System::KnownUserProperties::AccountName()));

            if (username.empty())
            {
                co_await CreateContentDialog(root, L"Error", L"Can't get the account name for the current user.", false, true).ShowAsync();
                co_return false;
            }

            switch ((co_await Windows::Security::Credentials::KeyCredentialManager::RequestCreateAsync(username, Windows::Security::Credentials::KeyCredentialCreationOption::ReplaceExisting)).Status())
            {
                case Windows::Security::Credentials::KeyCredentialStatus::Success:
                    // Armazenar a chave de configuração indicando que a senha foi inserida com sucesso.
                    local_settings.Values().Insert(SECURITY_KEY_SET_ID, box_value(true));
                    co_return true;

                default: break;
            }

            co_return false;
        }

        co_return true;
	}
}