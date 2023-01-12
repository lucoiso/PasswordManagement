// Author: Lucas Oliveira Vilas-Bôas
// Year: 2022
// Repository: https://github.com/lucoiso/PasswordManagement

#pragma once

#include "LoginUpdateEventParams.g.h"

namespace winrt::PasswordManager::implementation
{
    struct LoginUpdateEventParams : LoginUpdateEventParamsT<LoginUpdateEventParams>
    {
        LoginUpdateEventParams() = delete;
        LoginUpdateEventParams(winrt::PasswordManager::LoginData const& data, winrt::PasswordManager::LoginDataFileType const& type);

        winrt::PasswordManager::LoginData Data() const;
        winrt::PasswordManager::LoginDataFileType Type() const;

    private:
        winrt::PasswordManager::LoginData m_data;
        winrt::PasswordManager::LoginDataFileType m_type;
    };
}
namespace winrt::PasswordManager::factory_implementation
{
    struct LoginUpdateEventParams : LoginUpdateEventParamsT<LoginUpdateEventParams, implementation::LoginUpdateEventParams>
    {
    };
}
