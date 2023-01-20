// Author: Lucas Oliveira Vilas-Bôas
// Year: 2022
// Repository: https://github.com/lucoiso/PasswordManagement

#pragma once

#include "MainPage.g.h"

namespace winrt::MainApplication::implementation
{
    struct MainPage : MainPageT<MainPage>
    {
        MainPage();

        void InitializeSubobjects();

    protected:
        Windows::Foundation::IAsyncAction performDataImport();
        Windows::Foundation::IAsyncAction performDataExport();

    private:
        PasswordManager::LoginDataManager m_manager;
        event_token m_data_update_token;
        event_token m_data_import_token;
        event_token m_data_export_token;

        void bindDataUpdate();
        void bindDataLoad();
        
        Windows::Foundation::IAsyncAction loadLocalData();
        Windows::Foundation::IAsyncAction saveLocalData();
    };
}

namespace winrt::MainApplication::factory_implementation
{
    struct MainPage : MainPageT<MainPage, implementation::MainPage>
    {
    };
}
