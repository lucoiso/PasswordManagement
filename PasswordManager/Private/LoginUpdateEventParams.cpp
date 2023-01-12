// Author: Lucas Oliveira Vilas-Bôas
// Year: 2022
// Repository: https://github.com/lucoiso/PasswordManagement

#include "pch.h"

#include "LoginUpdateEventParams.h"
#include "LoginUpdateEventParams.g.cpp"

namespace winrt::PasswordManager::implementation
{
    LoginUpdateEventParams::LoginUpdateEventParams(winrt::PasswordManager::LoginData const& data, winrt::PasswordManager::LoginDataFileType const& type) : m_data(data), m_type(type)
    {
    }

    winrt::PasswordManager::LoginData LoginUpdateEventParams::Data() const
    {
        return this->m_data;
    }

    winrt::PasswordManager::LoginDataFileType LoginUpdateEventParams::Type() const
    {
        return this->m_type;
    }
}
