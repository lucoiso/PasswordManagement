// Author: Lucas Oliveira Vilas-Bôas
// Year: 2022
// Repository: https://github.com/lucoiso/PasswordManagement

#include "pch.h"

#include "LoginDataListView.xaml.h"
#include "LoginDataListView.g.cpp"

#include <Helper.h>

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
        const winrt::PasswordManager::LoginData newData = data.Clone().try_as<winrt::PasswordManager::LoginData>();
		winrt::check_bool(newData != nullptr);

        if (const auto matchingIterator = std::find_if(this->m_data.begin(), this->m_data.end(), [newData](const winrt::PasswordManager::LoginData& element) { return element.Equals(newData); });
            matchingIterator != this->m_data.end())
        {
            const unsigned int index = static_cast<unsigned int>(std::distance(this->m_data.begin(), matchingIterator));
            this->m_data.SetAt(index, newData);
        }
        else
        {
            this->m_data.Append(newData);
        }
        
        TB_EntriesNum().Text(L"Entries: " + winrt::to_hstring(this->m_data.Size()));
    }

    winrt::Windows::Foundation::Collections::IObservableVector<winrt::PasswordManager::LoginData> LoginDataListView::Data() const
    {
        return this->m_data;
    }

    void LoginDataListView::Sort([[maybe_unused]] winrt::MainApplication::DataSortMode const& mode, [[maybe_unused]] winrt::MainApplication::DataSortOrientation const& orientation)
    {
        // TODO
    }
}
