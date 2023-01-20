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

        m_data = single_threaded_observable_vector<PasswordManager::LoginData>();
    }

    void LoginDataListView::insertDataInList(PasswordManager::LoginData const& data)
    {
        const PasswordManager::LoginData newData = data.Clone().try_as<PasswordManager::LoginData>();
		check_bool(newData != nullptr);

        if (const auto matchingIterator = std::find_if(m_data.begin(), m_data.end(), [newData](const PasswordManager::LoginData& element) { return element.Equals(newData); });
            matchingIterator != m_data.end())
        {
            const unsigned int index = static_cast<unsigned int>(std::distance(m_data.begin(), matchingIterator));
            m_data.SetAt(index, newData);
        }
        else
        {
            m_data.Append(newData);
        }
        
        TB_EntriesNum().Text(L"Entries: " + to_hstring(m_data.Size()));
    }

    Windows::Foundation::Collections::IObservableVector<PasswordManager::LoginData> LoginDataListView::Data() const
    {
        return m_data;
    }

    void LoginDataListView::Sort([[maybe_unused]] MainApplication::DataSortMode const& mode, [[maybe_unused]] MainApplication::DataSortOrientation const& orientation)
    {
        // TODO
    }
}
