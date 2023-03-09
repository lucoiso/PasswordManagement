// Author: Lucas Oliveira Vilas-Bôas
// Year: 2023
// Repository: https://github.com/lucoiso/PasswordManagement

#pragma once

#include "LoginUpdateEventParams.g.h"

namespace winrt::PasswordManager::implementation
{
    struct LoginUpdateEventParams : LoginUpdateEventParamsT<LoginUpdateEventParams>
    {
        LoginUpdateEventParams() = delete;
        LoginUpdateEventParams(PasswordManager::LoginData const& data, PasswordManager::LoginDataExportType const& type);

        PasswordManager::LoginData Data() const;
        PasswordManager::LoginDataExportType Type() const;

    private:
        PasswordManager::LoginData m_data;
        PasswordManager::LoginDataExportType m_type;
    };
}
namespace winrt::PasswordManager::factory_implementation
{
    struct LoginUpdateEventParams : LoginUpdateEventParamsT<LoginUpdateEventParams, implementation::LoginUpdateEventParams>
    {
    };
}
