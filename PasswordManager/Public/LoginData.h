// Author: Lucas Oliveira Vilas-Bôas
// Year: 2022
// Repository: https://github.com/lucoiso/PasswordManagement

#pragma once

#include "LoginData.g.h"

namespace winrt::PasswordManager::implementation
{    
    struct LoginData : LoginDataT<LoginData>
    {
        LoginData() = default;
        inline bool operator==(const LoginData& rhs) const
        {
            return Url() == rhs.Url() && Username() == rhs.Username();
        }

        PasswordManager::IFunctionalData Clone() const;
        const bool Equals(PasswordManager::IFunctionalData const& rhs) const;

#pragma region Getters and Setters
        hstring Name() const;
        void Name(hstring const& value);

        hstring Url() const;
        void Url(hstring const& value);

        hstring Username() const;
        void Username(hstring const& value);

        hstring Password() const;
        void Password(hstring const& value);
#pragma endregion Getters and Setters

        hstring GetDataAsString(PasswordManager::LoginDataFileType const& type) const;

        bool HasEmptyData() const;

        void resetLoginData();

    protected:
        hstring GetDataAsString_CSV_Internal() const;
        hstring GetDataAsString_TXT_Internal() const;

    private:
        hstring m_name;
        hstring m_url;
        hstring m_username;
        hstring m_password;
    };
}
namespace winrt::PasswordManager::factory_implementation
{
    struct LoginData : LoginDataT<LoginData, implementation::LoginData>
    {
    };
}
