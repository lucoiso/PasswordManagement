// Author: Lucas Oliveira Vilas-Bôas
// Year: 2023
// Repository: https://github.com/lucoiso/PasswordManagement

#pragma once

#include "MainPage.g.h"

namespace winrt::MainApplication::implementation
{
    struct MainPage : MainPageT<MainPage>
    {
        MainPage();

        bool EnableLicenseTools() const;

        Windows::Foundation::IAsyncAction OnNavigatedTo(Microsoft::UI::Xaml::Navigation::NavigationEventArgs const& e);

        Windows::Foundation::IAsyncAction InsertLoginData(PasswordManager::LoginData const& data, const bool save_data);
        Windows::Foundation::IAsyncAction RemoveLoginData(PasswordManager::LoginData const& data, const bool save_data);

        Windows::Foundation::IAsyncAction RemoveAllLoginData(const bool save_data);

        Windows::Foundation::IAsyncAction LoadLocalDataAsync();
        Windows::Foundation::IAsyncAction SaveLocalDataAsync();

        Windows::Foundation::IAsyncAction ImportDataFromFileAsync(const Windows::Storage::StorageFile& data_file, const bool save_data);
        Windows::Foundation::IAsyncAction ReplaceDataWithFileAsync(const Windows::Storage::StorageFile& data_file, const bool save_data);

    protected:
        Windows::Foundation::IAsyncAction PerformDataImportAsync();
        Windows::Foundation::IAsyncAction PerformDataExportAsync(const PasswordManager::LoginDataExportType export_type);

    private:
        bool m_enable_license_tools{ false };

        PasswordManager::LoginDataManager m_manager;
        event_token m_data_update_token;
        event_token m_data_import_token;
        event_token m_data_export_token;
        event_token m_data_sort_token;

        void BindDataUpdate();
        void BindDataLoad();
        void BindDataSorting();
    };
}

namespace winrt::MainApplication::factory_implementation
{
    struct MainPage : MainPageT<MainPage, implementation::MainPage>
    {
    };
}
