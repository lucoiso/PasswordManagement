// Author: Lucas Oliveira Vilas-Bôas
// Year: 2023
// Repository: https://github.com/lucoiso/PasswordManagement

#pragma once

#include "LoginDataManager.g.h"

namespace winrt::PasswordManager::implementation
{
    struct LoginDataManager : LoginDataManagerT<LoginDataManager>
    {
        LoginDataManager() = default;

        Windows::Foundation::IAsyncAction ImportDataAsync(const Windows::Storage::StorageFile& file, const PasswordManager::LoginDataExportType export_type);
        Windows::Foundation::IAsyncAction ExportDataAsync(const Windows::Storage::StorageFile& file, const Windows::Foundation::Collections::IVectorView<PasswordManager::LoginData>& data, const PasswordManager::LoginDataExportType export_type);

        event_token LoginDataUpdated(Windows::Foundation::EventHandler<PasswordManager::LoginUpdateEventParams> const& handler);
        void LoginDataUpdated(event_token const& token) noexcept;

    private:
        event<Windows::Foundation::EventHandler<PasswordManager::LoginUpdateEventParams>> m_data_updated;
        Windows::Security::Cryptography::Core::CryptographicKey* m_cryptographic_key{ nullptr };

    protected:
        void PushData(const PasswordManager::LoginData& data, const PasswordManager::LoginDataExportType& type);

        Windows::Foundation::IAsyncAction ReadData_Lupass(const Windows::Storage::StorageFile& file);
        Windows::Foundation::IAsyncAction WriteData_Lupass(const Windows::Storage::StorageFile& file, const Windows::Foundation::Collections::IVectorView<PasswordManager::LoginData>& data);

        void ReadData_GenericCSV(const Windows::Foundation::Collections::IVectorView<hstring>& file_text, const PasswordManager::LoginDataExportType& data_type);
        Windows::Foundation::IAsyncAction WriteData_GenericCSV(const Windows::Storage::StorageFile& file, const Windows::Foundation::Collections::IVectorView<PasswordManager::LoginData>& data, const PasswordManager::LoginDataExportType& data_type) const;

        void ProcessCsvLine(const hstring line, PasswordManager::LoginData& current_data, const PasswordManager::LoginDataExportType& data_type);

        void ReadData_Kapersky(const Windows::Foundation::Collections::IVectorView<hstring>& file_text);
        Windows::Foundation::IAsyncAction WriteData_Kapersky(const Windows::Storage::StorageFile& file, const Windows::Foundation::Collections::IVectorView<PasswordManager::LoginData>& data) const;
    };
}
namespace winrt::PasswordManager::factory_implementation
{
    struct LoginDataManager : LoginDataManagerT<LoginDataManager, implementation::LoginDataManager>
    {
    };
}
