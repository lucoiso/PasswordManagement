// Author: Lucas Oliveira Vilas-Bôas
// Year: 2023
// Repository: https://github.com/lucoiso/PasswordManagement

#include "pch.h"

#include "LoginDataListView.xaml.h"
#include "LoginDataListView.g.cpp"

#include "App.xaml.h"

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MainApplication::implementation
{
    LoginDataListView::LoginDataListView()
    {
        InitializeComponent();

        m_data = single_threaded_vector<PasswordManager::LoginData>();
        m_filtered_data = single_threaded_observable_vector<PasswordManager::LoginData>();

        auto main_window = Application::Current().as<winrt::MainApplication::implementation::App>()->Window();
        UpdateListSize(main_window.Bounds().Width, main_window.Bounds().Height);

        main_window.SizeChanged({ this, &LoginDataListView::WindowSize_Changed });
    }

    void LoginDataListView::UpdateListSize(const float width, const float height)
    {
        LUPASS_LOG_FUNCTION();

        const float list_width = width * 0.6f;
        const float list_height = height * 0.8f;

        LV_DataList().Width(list_width);
        LV_DataList().Height(list_height);

        TB_Search().Width(list_width);
    }

    void LoginDataListView::WindowSize_Changed([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] Microsoft::UI::Xaml::WindowSizeChangedEventArgs const& args)
    {
        LUPASS_LOG_FUNCTION();

        UpdateListSize(args.Size().Width, args.Size().Height);
    }

    bool LoginDataListView::EnableLicenseTools() const
    {
        return m_enable_license_tools;
    }

    void LoginDataListView::EnableLicenseTools(bool value)
    {
        Helper::SetMemberValue(value, m_enable_license_tools);
    }

    void LoginDataListView::InsertDataInList(PasswordManager::LoginData const& data)
    {
        LUPASS_LOG_FUNCTION();

        PasswordManager::LoginData newData = data.Clone().try_as<PasswordManager::LoginData>();
		check_bool(newData != nullptr);

        newData.InitializeInvalidTimes();
        
		const auto push_data = [&newData](const auto& container, const auto& validator)
        {
            if (const auto matchingIterator = std::find_if(container.begin(), container.end(), [newData](const PasswordManager::LoginData& element) { return element.Equals(newData); });
                matchingIterator != container.end())
            {
                const unsigned int index = static_cast<unsigned int>(std::distance(container.begin(), matchingIterator));
                                
				if (newData.Name().size() <= 0 || (Helper::GetCleanUrlString(newData.Name()) == newData.Url() && container.GetAt(index).Name().size() > 0))
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

    void LoginDataListView::RemoveDataFromList(PasswordManager::LoginData const& data)
    {
        LUPASS_LOG_FUNCTION();

        PasswordManager::LoginData newData = data.Clone().try_as<PasswordManager::LoginData>();
        check_bool(newData != nullptr);

        const auto remove_data = [&newData](const auto& container)
        {
            if (const auto matchingIterator = std::find_if(container.begin(), container.end(), [newData](const PasswordManager::LoginData& element) { return element.Equals(newData); });
                matchingIterator != container.end())
            {
				container.RemoveAt(static_cast<unsigned int>(std::distance(container.begin(), matchingIterator)));
			}
		};

		remove_data(m_data);
		remove_data(m_filtered_data);

		UpdateEntriesIndicator();
    }

    void LoginDataListView::RemoveAllDataFromList()
    {
        LUPASS_LOG_FUNCTION();

        m_data.Clear();
		m_filtered_data.Clear();
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
        LUPASS_LOG_FUNCTION();

        UpdateFilteredData();
    }

    void MainApplication::implementation::LoginDataListView::Sort(DataSortMode const& mode, DataSortOrientation const& orientation)
    {
        LUPASS_LOG_FUNCTION();

		std::vector<PasswordManager::LoginData> local_data(m_data.Size());
		m_data.GetMany(0u, local_data);
        m_data.Clear();
        
        std::sort(local_data.begin(), local_data.end(), [&](const PasswordManager::LoginData& lhs, const PasswordManager::LoginData& rhs) -> bool {
            const auto filter_orientation = [&](const auto& left_string, const auto& right_string) -> bool
            {
                switch (orientation)
                {
                    case DataSortOrientation::Ascending:
                        return left_string < right_string;

                    case DataSortOrientation::Descending:
                        return left_string > right_string;

                    default:
                        return false;
                }
            };

            switch (mode)
            {
                case DataSortMode::Name:
                    return filter_orientation(lhs.Name(), rhs.Name());

                case DataSortMode::Url:
                    return filter_orientation(lhs.Url(), rhs.Url());

                case DataSortMode::Username:
                    return filter_orientation(lhs.Username(), rhs.Username());

                case DataSortMode::Notes:
                    return filter_orientation(lhs.Notes(), rhs.Notes());

                case DataSortMode::Created:
                    return filter_orientation(lhs.Created(), rhs.Created());

                case DataSortMode::Used:
                    return filter_orientation(lhs.Used(), rhs.Used());

                case DataSortMode::Changed:
                    return filter_orientation(lhs.Changed(), rhs.Changed());

                default:
                    return false;
            }
        });

		m_data = single_threaded_vector(std::move(local_data));

        UpdateFilteredData();
    }
    
    bool MainApplication::implementation::LoginDataListView::MatchSearch(const PasswordManager::LoginData& data) const
    {
        LUPASS_LOG_FUNCTION();

        return Helper::StringContains(data.Name(), m_current_search) 
            || Helper::StringContains(data.Url(), m_current_search) 
            || Helper::StringContains(data.Username(), m_current_search)
            || Helper::StringContains(data.Notes(), m_current_search);
    }

    void MainApplication::implementation::LoginDataListView::UpdateFilteredData()
    {
        LUPASS_LOG_FUNCTION();

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

    void MainApplication::implementation::LoginDataListView::UpdateEntriesIndicator()
    {
        LUPASS_LOG_FUNCTION();

        TB_TotalEntriesNum().Text(L"Total Entries: " + to_hstring(m_data.Size()));
        TB_FilteredEntriesNum().Text(L"Filtered Entries: " + to_hstring(m_filtered_data.Size()));
    }
}
