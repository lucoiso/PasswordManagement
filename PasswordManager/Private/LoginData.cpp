// Author: Lucas Oliveira Vilas-Bôas
// Year: 2022
// Repository: https://github.com/lucoiso/PasswordManagement

#include "pch.h"

#include "LoginData.h"
#include "LoginData.g.cpp"

#include "Helper.h"

namespace winrt::PasswordManager::implementation
{
#pragma region Getters and Setters
    hstring LoginData::Name() const
    {
        return this->m_name;
    }

    void LoginData::Name(hstring const& value)
    {
        Helper::setMemberValue(value, this->m_name);
    }

    hstring LoginData::Url() const
    {
        return this->m_url;
    }

    void LoginData::Url(hstring const& value)
    {
        Helper::setMemberValue(value, this->m_url);
    }

    hstring LoginData::Username() const
    {
        return this->m_username;
    }

    void LoginData::Username(hstring const& value)
    {
        Helper::setMemberValue(value, this->m_username);
    }

    hstring LoginData::Password() const
    {
        return this->m_password;
    }

    void LoginData::Password(hstring const& value)
    {
        Helper::setMemberValue(value, this->m_password);
    }
#pragma endregion Getters and Setters

    hstring LoginData::getDataAsString(winrt::PasswordManager::LoginDataFileType const& inType) const
    {
        switch (inType)
        {
        case winrt::PasswordManager::LoginDataFileType::CSV:
            return getDataAsString_CSV_Internal();

        case winrt::PasswordManager::LoginDataFileType::TXT:
            return getDataAsString_TXT_Internal();

        default:
            throw hresult_not_implemented(L"not implemented yet");
            break;
        }
    }

    bool LoginData::hasEmptyData() const
    {
        return Helper::hasEmptyData(this->m_name, this->m_url, this->m_username, this->m_password);
    }

    void LoginData::resetLoginData()
    {
        this->m_name.clear();
        this->m_url.clear();
        this->m_username.clear();
        this->m_password.clear();
    }

    hstring LoginData::getDataAsString_CSV_Internal() const
    {
        return this->m_name + L"," + this->m_url + L"," + this->m_username + L"," + this->m_password;
    }

    hstring LoginData::getDataAsString_TXT_Internal() const
    {
        return L"Website name: " + this->m_name + L"\nWebsite URL: " + this->m_url + L"\nLogin name: \nLogin: " + this->m_username + L"\nPassword: " + this->m_password + L"\nComment: \n\n---\n\n";
    }
}
