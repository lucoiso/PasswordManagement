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

        void InsertDataInList(PasswordManager::LoginData const& data);
        Windows::Foundation::Collections::IObservableVector<PasswordManager::LoginData> FilteredData() const;
        Windows::Foundation::Collections::IVector<PasswordManager::LoginData> Data() const;

        void TB_Search_TextChanged(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::Controls::TextChangedEventArgs const& args);

    private:
        Windows::Foundation::Collections::IVector<PasswordManager::LoginData> m_data;
        Windows::Foundation::Collections::IObservableVector<PasswordManager::LoginData> m_filtered_data;

        hstring m_current_search;

    protected:
		bool MatchSearch(const PasswordManager::LoginData& data) const;
        void UpdateEntriesIndicator();
    };
}

namespace winrt::MainApplication::factory_implementation
{
    struct LoginDataListView : LoginDataListViewT<LoginDataListView, implementation::LoginDataListView>
    {
    };
}
