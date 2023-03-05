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

        void LI_LoginData_Loaded(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args);

        void AddLoginData(PasswordManager::LoginData const& data);
        void RemoveLoginData(PasswordManager::LoginData const& data);

        void RemoveAllLoginData();

    protected:
        Windows::Foundation::IAsyncAction PerformDataImportAsync();
        Windows::Foundation::IAsyncAction PerformDataExportAsync();

        Windows::Foundation::IAsyncAction LoadLocalDataAsync();
        Windows::Foundation::IAsyncAction SaveLocalDataAsync();

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
