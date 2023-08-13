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

        Windows::Foundation::IAsyncAction ImportDataAsync(Windows::Storage::StorageFile const file, const PasswordManager::LoginDataFileType import_type);
        Windows::Foundation::IAsyncAction ExportDataAsync(Windows::Storage::StorageFile const file, Windows::Foundation::Collections::IVectorView<PasswordManager::LoginData> const& data, const PasswordManager::LoginDataFileType export_type);

        event_token LoginDataUpdated(Windows::Foundation::EventHandler<PasswordManager::LoginUpdateEventParams> const& handler);
        void LoginDataUpdated(event_token const& token) noexcept;

    private:
        event<Windows::Foundation::EventHandler<PasswordManager::LoginUpdateEventParams>> m_data_updated;

    protected:
        void PushData(std::vector<PasswordManager::LoginData>& data, const PasswordManager::LoginDataFileType type);

        Windows::Foundation::IAsyncOperation<Windows::Foundation::Collections::IVector<PasswordManager::LoginData>> ReadData_Lupass(hstring const& display_name, Windows::Storage::Streams::IBuffer const& data_buffer);
        Windows::Foundation::IAsyncAction WriteData_Lupass(Windows::Storage::StorageFile const& file, Windows::Foundation::Collections::IVectorView<PasswordManager::LoginData> const& data);

        std::vector<PasswordManager::LoginData> ReadData_GenericCSV(Windows::Foundation::Collections::IVectorView<hstring> const& file_text, const PasswordManager::LoginDataFileType data_type);
        Windows::Foundation::IAsyncAction WriteData_GenericCSV(Windows::Storage::StorageFile const& file, Windows::Foundation::Collections::IVectorView<PasswordManager::LoginData> const& data, const PasswordManager::LoginDataFileType data_type) const;

        void ProcessCsvLine(hstring const& line, std::vector<PasswordManager::LoginData>& output_data, const PasswordManager::LoginDataFileType data_type);

        std::vector<PasswordManager::LoginData> ReadData_Kapersky(Windows::Foundation::Collections::IVectorView<hstring> const& file_text);
        Windows::Foundation::IAsyncAction WriteData_Kapersky(Windows::Storage::StorageFile const& file, Windows::Foundation::Collections::IVectorView<PasswordManager::LoginData> const& data) const;
    };
}
namespace winrt::PasswordManager::factory_implementation
{
    struct LoginDataManager : LoginDataManagerT<LoginDataManager, implementation::LoginDataManager>
    {
    };
}
