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

        m_data = single_threaded_vector<PasswordManager::LoginData>();
        m_filtered_data = single_threaded_observable_vector<PasswordManager::LoginData>();
    }

    void LoginDataListView::InsertDataInList(PasswordManager::LoginData const& data)
    {
        PasswordManager::LoginData newData = data.Clone().try_as<PasswordManager::LoginData>();
		check_bool(newData != nullptr);
        
		const auto push_data = [&newData](const auto& container, const auto& validator)
        {
            if (const auto matchingIterator = std::find_if(container.begin(), container.end(), [newData](const PasswordManager::LoginData& element) { return element.Equals(newData); });
                matchingIterator != container.end())
            {
                const unsigned int index = static_cast<unsigned int>(std::distance(container.begin(), matchingIterator));
                                
				if (newData.Name().size() <= 0 
                    || (Helper::GetCleanUrlString(newData.Name()) == newData.Url() && container.GetAt(index).Name().size() > 0))
				{
					newData.Name(container.GetAt(index).Name());
				}

                container.SetAt(index, newData);
            }
			else if (validator(newData))
			{
				container.Append(newData);
			}
        };
        
        push_data(m_data, []([[maybe_unused]] const PasswordManager::LoginData& data) -> bool { return true; });
		push_data(m_filtered_data, [this](const PasswordManager::LoginData& data) { return MatchSearch(data); });
        
        UpdateEntriesIndicator();
    }

    Windows::Foundation::Collections::IObservableVector<PasswordManager::LoginData> MainApplication::implementation::LoginDataListView::FilteredData() const
    {
        return m_filtered_data;
    }

    Windows::Foundation::Collections::IVector<PasswordManager::LoginData> LoginDataListView::Data() const
    {
        return m_data;
    }

    void MainApplication::implementation::LoginDataListView::TB_Search_TextChanged([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] Controls::TextChangedEventArgs const& args)
    {
        m_current_search = TB_Search().Text();
        m_filtered_data.Clear();

        for (const auto& it : m_data)
        {
            if (MatchSearch(it))
            {
                m_filtered_data.Append(it);
            }
        }

        UpdateEntriesIndicator();
    }
    
    bool MainApplication::implementation::LoginDataListView::MatchSearch(const PasswordManager::LoginData& data) const
    {
        return Helper::StringContains(data.Name(), m_current_search) || Helper::StringContains(data.Url(), m_current_search) || Helper::StringContains(data.Username(), m_current_search);
    }

    void MainApplication::implementation::LoginDataListView::UpdateEntriesIndicator()
    {
        TB_TotalEntriesNum().Text(L"Total Entries: " + to_hstring(m_data.Size()));
        TB_FilteredEntriesNum().Text(L"Filtered Entries: " + to_hstring(m_filtered_data.Size()));
    }
}
