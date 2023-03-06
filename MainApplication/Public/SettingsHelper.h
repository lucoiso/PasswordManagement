// Author: Lucas Oliveira Vilas-Bôas
// Year: 2023
// Repository: https://github.com/lucoiso/PasswordManagement

#pragma once

#ifndef LUPASS_SETTINGS_HELPER_H
#define LUPASS_SETTINGS_HELPER_H

#include "pch.h"

using namespace winrt::MainApplication;

namespace winrt::Helper
{
    template <typename SettingTy>
    inline SettingTy GetSettingValue(const hstring& key)
    {
        LUPASS_LOG_FUNCTION();

        if (const auto local_settings = Windows::Storage::ApplicationData::Current().LocalSettings(); local_settings.Values().HasKey(key))
        {
			return local_settings.Values().Lookup(key).as<SettingTy>();
		}

		return SettingTy();
    }

    inline bool HasSettingKey(const hstring& key)
    {
        LUPASS_LOG_FUNCTION();

        const auto local_settings = Windows::Storage::ApplicationData::Current().LocalSettings();
        return local_settings.Values().HasKey(key);
    }

    template <typename SettingTy>
    inline void InsertSettingValue(const hstring& key, const SettingTy& value)
    {
        LUPASS_LOG_FUNCTION();

        const auto local_settings = Windows::Storage::ApplicationData::Current().LocalSettings();
        local_settings.Values().Insert(key, box_value(value));
    }

    inline void RemoveSettingKey(const hstring& key)
    {
        LUPASS_LOG_FUNCTION();

        const auto local_settings = Windows::Storage::ApplicationData::Current().LocalSettings();

        if (HasSettingKey(key))
        {
            local_settings.Values().Remove(key);
        }
    }

    inline Windows::Foundation::IAsyncAction InitializeSettings()
    {
        LUPASS_LOG_FUNCTION();

        const auto initialize_setting = [](const hstring& key, const auto& value)
        {
            if (!HasSettingKey(key))
            {
                InsertSettingValue(key, value);
            }
        };

        initialize_setting(SETTING_ENABLE_SYSTEM_TRAY, true);
        initialize_setting(SETTING_ENABLE_SHORTCUTS, true);

        initialize_setting(SETTING_GENERATOR_LAST_GENERATED_PASSWORD, L"Password");
        initialize_setting(SETTING_GENERATOR_ENABLE_LOWERCASE, true);
        initialize_setting(SETTING_GENERATOR_ENABLE_UPPERCASE, true);
        initialize_setting(SETTING_GENERATOR_ENABLE_NUMBERS, true);
        initialize_setting(SETTING_GENERATOR_ENABLE_CUSTOM_CHARACTERS, true);
        initialize_setting(SETTING_GENERATOR_PASSWORD_SIZE, 8);
        initialize_setting(SETTING_GENERATOR_CUSTOM_CHARACTERS, L",.;:[]{}!\'\"?");

        initialize_setting(SETTING_ENABLE_WINDOWS_HELLO, co_await Windows::Security::Credentials::KeyCredentialManager::IsSupportedAsync());
    }

    inline Windows::Foundation::IAsyncAction SetSettingsToDefault()
    {
        LUPASS_LOG_FUNCTION();

        InsertSettingValue(SETTING_ENABLE_SYSTEM_TRAY, true);
        InsertSettingValue(SETTING_ENABLE_SHORTCUTS, true);

        InsertSettingValue(SETTING_GENERATOR_LAST_GENERATED_PASSWORD, L"Password");
        InsertSettingValue(SETTING_GENERATOR_ENABLE_LOWERCASE, true);
        InsertSettingValue(SETTING_GENERATOR_ENABLE_UPPERCASE, true);
        InsertSettingValue(SETTING_GENERATOR_ENABLE_NUMBERS, true);
        InsertSettingValue(SETTING_GENERATOR_ENABLE_CUSTOM_CHARACTERS, true);
        InsertSettingValue(SETTING_GENERATOR_PASSWORD_SIZE, 8);
        InsertSettingValue(SETTING_GENERATOR_CUSTOM_CHARACTERS, L",.;:[]{}!\'\"?");

        InsertSettingValue(SETTING_ENABLE_WINDOWS_HELLO, co_await Windows::Security::Credentials::KeyCredentialManager::IsSupportedAsync());
    }

    inline void ClearSecurityIds()
    {
        LUPASS_LOG_FUNCTION();

        RemoveSettingKey(SECURITY_KEY_SET_ID);
        RemoveSettingKey(INVALID_SECURITY_ENVIRONMENT_ID);
    }
}
#endif
