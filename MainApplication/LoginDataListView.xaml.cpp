// Author: Lucas Oliveira Vilas-Bôas
// Year: 2023
// Repository: https://github.com/lucoiso/PasswordManagement

#include "pch.h"

#include "LoginDataListView.xaml.h"
#include "LoginDataListView.g.cpp"

#include "App.xaml.h"

#include "DataManager.h"

using namespace winrt;
using namespace Microsoft::UI::Xaml;

constexpr float s_width_percent = 0.6f;
constexpr float s_height_percent = 0.8f;

namespace winrt::MainApplication::implementation
{
    LoginDataListView::LoginDataListView()
    {
        InitializeComponent();

        m_data = single_threaded_observable_vector<PasswordManager::LoginData>();
    }

    void LoginDataListView::ComponentLoaded([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] Microsoft::UI::Xaml::RoutedEventArgs const& args)
    {
        const auto main_window = Application::Current().as<App>()->Window();
        UpdateListSize(main_window.Bounds().Width, main_window.Bounds().Height);

        main_window.SizeChanged(
            [this]([[maybe_unused]] auto&&, auto&& args)
            {
                UpdateListSize(args.Size().Width, args.Size().Height);
            }
        );

        // TODO: Perform the sync update in background to avoid UI freezing
        // TODO: Refactor to update only the sync data instead the entire list
        DataManager::GetInstance().DataSync(
            [this]()
            {
                m_data.Clear();

                const auto data_container = DataManager::GetInstance().Data(true);
                for (PasswordManager::LoginData const& data : data_container)
                {
					m_data.Append(data);
				}

                TB_TotalEntriesNum().Text(hstring(L"Total Entries: " + to_hstring(DataManager::GetInstance().DataSize())));
                TB_FilteredEntriesNum().Text(hstring(L"Filtered Entries: " + to_hstring(data_container.Size())));
            }
        );
    }

    void MainApplication::implementation::LoginDataListView::TB_Search_TextChanged([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] Controls::TextChangedEventArgs const& args)
    {
        LUPASS_LOG_FUNCTION();

        DataManager::GetInstance().SetSearchText(TB_Search().Text());
    }

    void LoginDataListView::UpdateListSize(const float width, const float height)
    {
        LUPASS_LOG_FUNCTION();

        const float list_width = width * s_width_percent;
        const float list_height = height * s_height_percent;

        LV_DataList().Width(list_width);
        LV_DataList().Height(list_height);

        TB_Search().Width(list_width);
    }

    Windows::Foundation::Collections::IObservableVector<PasswordManager::LoginData> LoginDataListView::Data() const
    {
        return m_data;
    }
}