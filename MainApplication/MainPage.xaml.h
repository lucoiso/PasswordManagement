// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#pragma once

#include "MainPage.g.h"

namespace winrt::MainApplication::implementation
{
    struct MainPage : MainPageT<MainPage>
    {
        MainPage();

        void InitializeSubobjects();

    protected:
        winrt::Windows::Foundation::IAsyncAction performDataImport();
        winrt::Windows::Foundation::IAsyncAction performDataExport();

    private:
        winrt::PasswordManager::LoginDataManager m_manager;
        winrt::event_token m_data_update_token;
        winrt::event_token m_data_import_token;
        winrt::event_token m_data_export_token;

        void bindDataUpdate();
        void bindDataLoad();
        
        winrt::Windows::Foundation::IAsyncAction loadLocalData();
        winrt::Windows::Foundation::IAsyncAction saveLocalData();
    };
}

namespace winrt::MainApplication::factory_implementation
{
    struct MainPage : MainPageT<MainPage, implementation::MainPage>
    {
    };
}
