// Author: Lucas Oliveira Vilas-Bôas
// Year: 2023
// Repository: https://github.com/lucoiso/PasswordManagement

#include "pch.h"

#include "LoginUpdateEventParams.h"
#include "LoginUpdateEventParams.g.cpp"

using namespace winrt;

namespace winrt::PasswordManager::implementation
{
    LoginUpdateEventParams::LoginUpdateEventParams(PasswordManager::LoginData const& data, PasswordManager::LoginDataFileType const& type) : m_data(data), m_type(type)
    {
    }

    PasswordManager::LoginData LoginUpdateEventParams::Data() const
    {
        LUPASS_LOG_FUNCTION();

        return m_data;
    }

    PasswordManager::LoginDataFileType LoginUpdateEventParams::Type() const
    {
        LUPASS_LOG_FUNCTION();

        return m_type;
    }
}
