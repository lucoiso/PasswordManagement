// Author: Lucas Oliveira Vilas-Bôas
// Year: 2023
// Repository: https://github.com/lucoiso/PasswordManagement

#include "pch.h"

#include "LoginData.h"
#include "LoginData.g.cpp"

using namespace winrt;

namespace winrt::PasswordManager::implementation
{
    PasswordManager::IFunctionalData LoginData::Clone() const
    {
        PasswordManager::LoginData output;
        output.Name(m_name);
        output.Url(m_url);
        output.Username(m_username);
        output.Password(m_password);

        return output;
    }

    const bool LoginData::Equals(PasswordManager::IFunctionalData const& rhs) const
    {
        return *this == *rhs.try_as<LoginData>().get();
    }

#pragma region Getters and Setters
    hstring LoginData::Name() const
    {
        return m_name;
    }

    void LoginData::Name(hstring const& value)
    {
        Helper::SetMemberValue(value, m_name);
    }

    hstring LoginData::Url() const
    {
        return m_url;
    }

    void LoginData::Url(hstring const& value)
    {
        Helper::SetMemberValue(Helper::GetCleanUrlString(value), m_url);
    }

    hstring LoginData::Username() const
    {
        return m_username;
    }

    void LoginData::Username(hstring const& value)
    {
        Helper::SetMemberValue(value, m_username);
    }

    hstring LoginData::Password() const
    {
        return m_password;
    }

    void LoginData::Password(hstring const& value)
    {
        std::string new_pass = to_string(value);
        if (std::equal(new_pass.begin(), new_pass.begin() + 1, "\"") && std::equal(new_pass.end() - 1, new_pass.end(), "\""))
        {
            new_pass = new_pass.substr(1, new_pass.size() - 2);
        }

        Helper::SetMemberValue(to_hstring(new_pass), m_password);
    }
#pragma endregion Getters and Setters

    hstring LoginData::GetDataAsString(PasswordManager::LoginDataFileType const& inType) const
    {
        switch (inType)
        {
            case PasswordManager::LoginDataFileType::CSV:
            case PasswordManager::LoginDataFileType::BIN:
                return GetDataAsString_CSV_Internal();

            case PasswordManager::LoginDataFileType::TXT:
                return GetDataAsString_TXT_Internal();

            default:
                throw hresult_not_implemented(L"not implemented yet");
                break;
        }
    }

    bool LoginData::HasEmptyData() const
    {
        return Helper::HasEmptyData(m_url, m_username, m_password);
    }

    void LoginData::resetLoginData()
    {
        m_name.clear();
        m_url.clear();
        m_username.clear();
        m_password.clear();
    }

    hstring LoginData::GetDataAsString_CSV_Internal() const
    {
        return Name() + L"," + L"https://" + Url() + L"/" + L"," + Username() + L"," + L"\"" + Password() + L"\"";
    }

    hstring LoginData::GetDataAsString_TXT_Internal() const
    {
        return L"Website name: " + Name() + L"\nWebsite URL: " + Url() + L"\nLogin name: \nLogin: " + Username() + L"\nPassword: " + Password() + L"\nComment: \n\n---\n\n";
    }
}
