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
            return this->Name() == rhs.Name() && this->Url() == rhs.Url() && this->Username() == rhs.Username();
        }

        winrt::PasswordManager::IFunctionalData Clone() const;
        const bool Equals(winrt::PasswordManager::IFunctionalData const& rhs) const;

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

        hstring getDataAsString(winrt::PasswordManager::LoginDataFileType const& type) const;

        bool hasEmptyData() const;

        void resetLoginData();

    private:
        hstring m_name;
        hstring m_url;
        hstring m_username;
        hstring m_password;

        hstring getDataAsString_CSV_Internal() const;
        hstring getDataAsString_TXT_Internal() const;
    };
}
namespace winrt::PasswordManager::factory_implementation
{
    struct LoginData : LoginDataT<LoginData, implementation::LoginData>
    {
    };
}
