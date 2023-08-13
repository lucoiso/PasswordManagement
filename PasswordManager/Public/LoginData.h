// Author: Lucas Oliveira Vilas-Bôas
// Year: 2023
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

        hstring Notes() const;
        void Notes(hstring const& value);

        uint64_t Created() const;
        void Created(uint64_t const& value);

        uint64_t Changed() const;
        void Changed(uint64_t const& value);

        uint64_t Used() const;
        void Used(uint64_t const& value);
#pragma endregion Getters and Setters

        void InitializeInvalidTimes();
        hstring GetExportData(PasswordManager::LoginDataFileType const& type) const;

        bool HasEmptyData() const;

        void ResetLoginData();

    protected:
        hstring GetExportData_Microsoft() const;
        hstring GetExportData_Google() const;
        hstring GetExportData_Firefox() const;
        hstring GetExportData_Lupass() const;
        hstring GetExportData_Kapersky() const;

    private:
        hstring m_name;
        hstring m_url;
        hstring m_username;
        hstring m_password;
        hstring m_notes;
        uint64_t m_created;
        uint64_t m_changed;
        uint64_t m_used;
    };
}
namespace winrt::PasswordManager::factory_implementation
{
    struct LoginData : LoginDataT<LoginData, implementation::LoginData>
    {
    };
}
