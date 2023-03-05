// Author: Lucas Oliveira Vilas-B�as
// Year: 2023
// Repository: https://github.com/lucoiso/PasswordManagement

#pragma once

#include "LoginDataListView.g.h"

namespace winrt::MainApplication::implementation
{
    struct LoginDataListView : LoginDataListViewT<LoginDataListView>
    {
        LoginDataListView();

        bool EnableLicenseTools() const;
        void EnableLicenseTools(bool value);

        void InsertDataInList(PasswordManager::LoginData const& data);
        void RemoveDataFromList(PasswordManager::LoginData const& data);
        void RemoveAllDataFromList();

        Windows::Foundation::Collections::IObservableVector<PasswordManager::LoginData> FilteredData() const;
        Windows::Foundation::Collections::IVector<PasswordManager::LoginData> Data() const;

        void TB_Search_TextChanged(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::Controls::TextChangedEventArgs const& args);

        void Sort(DataSortMode const& mode, DataSortOrientation const& orientation);

    private:
        bool m_enable_license_tools{ false };
        Windows::Foundation::Collections::IVector<PasswordManager::LoginData> m_data;
        Windows::Foundation::Collections::IObservableVector<PasswordManager::LoginData> m_filtered_data;

        hstring m_current_search;

    protected:
		bool MatchSearch(const PasswordManager::LoginData& data) const;
		void UpdateFilteredData();
        void UpdateEntriesIndicator();
    };
}

namespace winrt::MainApplication::factory_implementation
{
    struct LoginDataListView : LoginDataListViewT<LoginDataListView, implementation::LoginDataListView>
    {
    };
}
