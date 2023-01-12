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

        winrt::Windows::Foundation::IAsyncAction importData(const winrt::Windows::Storage::StorageFile& file);
        winrt::Windows::Foundation::IAsyncAction exportData(const winrt::Windows::Storage::StorageFile& file, const winrt::Windows::Foundation::Collections::IVectorView<winrt::PasswordManager::LoginData>& data);

        winrt::event_token LoginDataUpdated(winrt::Windows::Foundation::EventHandler<winrt::PasswordManager::LoginUpdateEventParams> const& handler);
        void LoginDataUpdated(winrt::event_token const& token) noexcept;

    private:
        event<winrt::Windows::Foundation::EventHandler<PasswordManager::LoginUpdateEventParams>> m_data_updated;

        void sendData(const winrt::PasswordManager::LoginData& data, const winrt::PasswordManager::LoginDataFileType& type);

        void readTextData(const winrt::Windows::Foundation::Collections::IVectorView<hstring>& file_text);
        winrt::Windows::Foundation::IAsyncAction writeTextData(const winrt::Windows::Storage::StorageFile& file, const winrt::Windows::Foundation::Collections::IVectorView<winrt::PasswordManager::LoginData>& data) const;

        void readCsvData(const winrt::Windows::Foundation::Collections::IVectorView<hstring>& file_text);
        winrt::Windows::Foundation::IAsyncAction writeCsvData(const winrt::Windows::Storage::StorageFile& file, const winrt::Windows::Foundation::Collections::IVectorView<winrt::PasswordManager::LoginData>& data) const;
    };
}
namespace winrt::PasswordManager::factory_implementation
{
    struct LoginDataManager : LoginDataManagerT<LoginDataManager, implementation::LoginDataManager>
    {
    };
}
