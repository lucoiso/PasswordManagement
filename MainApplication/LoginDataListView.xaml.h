// Author: Lucas Oliveira Vilas-Bôas
// Year: 2022
// Repository: https://github.com/lucoiso/PasswordManagement

#pragma once

#include "LoginDataListView.g.h"

namespace winrt::MainApplication::implementation
{
    struct LoginDataListView : LoginDataListViewT<LoginDataListView>
    {
        LoginDataListView();

        void insertDataInList(PasswordManager::LoginData const& data);
        Windows::Foundation::Collections::IObservableVector<PasswordManager::LoginData> Data() const;

        void Sort(MainApplication::DataSortMode const& mode, MainApplication::DataSortOrientation const& orientation);

    private:
        Windows::Foundation::Collections::IObservableVector<PasswordManager::LoginData> m_data;
    };
}

namespace winrt::MainApplication::factory_implementation
{
    struct LoginDataListView : LoginDataListViewT<LoginDataListView, implementation::LoginDataListView>
    {
    };
}
