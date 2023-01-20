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

        Windows::Foundation::IAsyncAction importData(const Windows::Storage::StorageFile& file);
        Windows::Foundation::IAsyncAction exportData(const Windows::Storage::StorageFile& file, const Windows::Foundation::Collections::IVectorView<PasswordManager::LoginData>& data);

        event_token LoginDataUpdated(Windows::Foundation::EventHandler<PasswordManager::LoginUpdateEventParams> const& handler);
        void LoginDataUpdated(event_token const& token) noexcept;

    private:
        event<Windows::Foundation::EventHandler<PasswordManager::LoginUpdateEventParams>> m_data_updated;

        void sendData(const PasswordManager::LoginData& data, const PasswordManager::LoginDataFileType& type);

        void readBinaryData(const Windows::Foundation::Collections::IVectorView<hstring>& file_text);
        Windows::Foundation::IAsyncAction writeBinaryData(const Windows::Storage::StorageFile& file, const Windows::Foundation::Collections::IVectorView<PasswordManager::LoginData>& data) const;

        void readTextData(const Windows::Foundation::Collections::IVectorView<hstring>& file_text);
        Windows::Foundation::IAsyncAction writeTextData(const Windows::Storage::StorageFile& file, const Windows::Foundation::Collections::IVectorView<PasswordManager::LoginData>& data) const;

        void readCsvData(const Windows::Foundation::Collections::IVectorView<hstring>& file_text);
        Windows::Foundation::IAsyncAction writeCsvData(const Windows::Storage::StorageFile& file, const Windows::Foundation::Collections::IVectorView<PasswordManager::LoginData>& data) const;

		void processCsvLine(const hstring line, PasswordManager::LoginData& current_data, const PasswordManager::LoginDataFileType& data_type);
    };
}
namespace winrt::PasswordManager::factory_implementation
{
    struct LoginDataManager : LoginDataManagerT<LoginDataManager, implementation::LoginDataManager>
    {
    };
}
