// Author: Lucas Oliveira Vilas-Bôas
// Year: 2022
// Repository: https://github.com/lucoiso/PasswordManagement

#pragma once

#include "winrt/Microsoft.UI.Xaml.h"
#include "winrt/Microsoft.UI.Xaml.Markup.h"
#include "winrt/Microsoft.UI.Xaml.Controls.Primitives.h"
#include "LoginDataListView.g.h"

namespace winrt::MainApplication::implementation
{
    struct LoginDataListView : LoginDataListViewT<LoginDataListView>
    {
        LoginDataListView();

        void insertDataInList(winrt::PasswordManager::LoginData const& data);
        winrt::Windows::Foundation::Collections::IObservableVector<winrt::PasswordManager::LoginData> Data() const;

    private:
        winrt::Windows::Foundation::Collections::IObservableVector<winrt::PasswordManager::LoginData> m_data;
    };
}

namespace winrt::MainApplication::factory_implementation
{
    struct LoginDataListView : LoginDataListViewT<LoginDataListView, implementation::LoginDataListView>
    {
    };
}
