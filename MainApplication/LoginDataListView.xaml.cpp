// Author: Lucas Oliveira Vilas-Bôas
// Year: 2022
// Repository: https://github.com/lucoiso/PasswordManagement

#include "pch.h"

#include "LoginDataListView.xaml.h"
#include "LoginDataListView.g.cpp"

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MainApplication::implementation
{
    LoginDataListView::LoginDataListView()
    {
        InitializeComponent();

        this->m_data = single_threaded_observable_vector<winrt::PasswordManager::LoginData>();
    }

    void LoginDataListView::insertDataInList(winrt::PasswordManager::LoginData const& data)
    {
        winrt::PasswordManager::LoginData newData_Copy;
        newData_Copy.Name(data.Name());
        newData_Copy.Url(data.Url());
        newData_Copy.Username(data.Username());
        newData_Copy.Password(data.Password());

        this->m_data.Append(newData_Copy);
    }

    winrt::Windows::Foundation::Collections::IObservableVector<winrt::PasswordManager::LoginData> LoginDataListView::Data() const
    {
        return this->m_data;
    }
}
