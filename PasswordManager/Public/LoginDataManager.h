// Author: Lucas Oliveira Vilas-Bôas
// Year: 2022
// Repository: https://github.com/lucoiso/PasswordManagement

#pragma once

#include "LoginDataManager.g.h"

namespace winrt::PasswordManager::implementation
{
    struct LoginDataManager : LoginDataManagerT<LoginDataManager>
    {
        LoginDataManager() = default;

        Windows::Foundation::IAsyncAction ImportDataAsync(const Windows::Storage::StorageFile& file);
        Windows::Foundation::IAsyncAction ExportDataAsync(const Windows::Storage::StorageFile& file, const Windows::Foundation::Collections::IVectorView<PasswordManager::LoginData>& data);

        event_token LoginDataUpdated(Windows::Foundation::EventHandler<PasswordManager::LoginUpdateEventParams> const& handler);
        void LoginDataUpdated(event_token const& token) noexcept;

    private:
        event<Windows::Foundation::EventHandler<PasswordManager::LoginUpdateEventParams>> m_data_updated;

    protected:
        void PushData(const PasswordManager::LoginData& data, const PasswordManager::LoginDataFileType& type);

        void ReadBinaryData(const Windows::Foundation::Collections::IVectorView<hstring>& file_text);
        Windows::Foundation::IAsyncAction WriteBinaryDataAsync(const Windows::Storage::StorageFile& file, const Windows::Foundation::Collections::IVectorView<PasswordManager::LoginData>& data) const;

        void ReadTextData(const Windows::Foundation::Collections::IVectorView<hstring>& file_text);
        Windows::Foundation::IAsyncAction WriteTextDataAsync(const Windows::Storage::StorageFile& file, const Windows::Foundation::Collections::IVectorView<PasswordManager::LoginData>& data) const;

        void ReadCsvData(const Windows::Foundation::Collections::IVectorView<hstring>& file_text);
        Windows::Foundation::IAsyncAction WriteCsvDataAsync(const Windows::Storage::StorageFile& file, const Windows::Foundation::Collections::IVectorView<PasswordManager::LoginData>& data) const;

		void ProcessCsvLine(const hstring line, PasswordManager::LoginData& current_data, const PasswordManager::LoginDataFileType& data_type);
    };
}
namespace winrt::PasswordManager::factory_implementation
{
    struct LoginDataManager : LoginDataManagerT<LoginDataManager, implementation::LoginDataManager>
    {
    };
}
