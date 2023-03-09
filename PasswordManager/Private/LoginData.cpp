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
        output.Notes(m_notes);

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

    hstring LoginData::Notes() const
    {
        return m_notes;
    }

    void LoginData::Notes(hstring const& value)
    {
        Helper::SetMemberValue(value, m_notes);
    }
#pragma endregion Getters and Setters

    hstring LoginData::GetExportData(PasswordManager::LoginDataExportType const& inType) const
    {
        switch (inType)
        {
            case PasswordManager::LoginDataExportType::Microsoft:
                return GetExportData_Microsoft();

            case PasswordManager::LoginDataExportType::Google:
                return GetExportData_Google();

            case PasswordManager::LoginDataExportType::Firefox:
                return GetExportData_Firefox();

            case PasswordManager::LoginDataExportType::Lupass:
                return GetExportData_Lupass();

            case PasswordManager::LoginDataExportType::Kapersky:
                return GetExportData_Kapersky();

            default:
                throw hresult_invalid_argument(L"Invalid data type");
                break;
        }
    }

    bool LoginData::HasEmptyData() const
    {
        return Helper::HasEmptyData(m_url, m_username, m_password);
    }

    void LoginData::ResetLoginData()
    {
        m_name.clear();
        m_url.clear();
        m_username.clear();
        m_password.clear();
        m_notes.clear();
    }

    hstring LoginData::GetExportData_Microsoft() const
    {
        return Name() + L"," + L"https://" + Url() + L"/" + L"," + Username() + L"," + L"\"" + Password() + L"\"";
    }

    hstring LoginData::GetExportData_Google() const
    {
        return Name() + L"," + L"https://" + Url() + L"/" + L"," + Username() + L"," + L"\"" + Password() + L"\"" + L"," + Notes();
    }

    hstring LoginData::GetExportData_Firefox() const
    {
        return L"https://" + Url() + L"/" + L"," + Username() + L"," + L"\"" + Password() + L"\"" + L",,,,";
    }

    hstring LoginData::GetExportData_Lupass() const
    {
        return GetExportData_Google();
    }

    hstring LoginData::GetExportData_Kapersky() const
    {
        return L"Website name: " + Name() + L"\nWebsite URL: " + Url() + L"\nLogin name: \nLogin: " + Username() + L"\nPassword: " + Password() + L"\nComment: " + Notes() + L"\n\n---\n\n";
    }
}
