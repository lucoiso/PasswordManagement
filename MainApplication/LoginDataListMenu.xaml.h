// Author: Lucas Oliveira Vilas-Bôas
// Year: 2023
// Repository: https://github.com/lucoiso/PasswordManagement

#pragma once

#include "LoginDataListMenu.g.h"

namespace winrt::MainApplication::implementation
{
    struct LoginDataListMenu : LoginDataListMenuT<LoginDataListMenu>
    {
        LoginDataListMenu();

        void BP_Import_Click(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args);
        Windows::Foundation::IAsyncAction BP_Export_Click(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args);
        Windows::Foundation::IAsyncAction BP_Add_Click(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args);
        Windows::Foundation::IAsyncAction BP_Clear_Click(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args);
       
        void GenericSorting_SelectionChanged(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::Controls::SelectionChangedEventArgs const& args);

        PasswordManager::LoginDataFileType SelectedExportDataType();
        MainApplication::DataSortMode SelectedSortingMode();
        MainApplication::DataSortOrientation SelectedSortingOrientation();

        event_token ImportPressed(MainApplication::SingleVoidDelegate const& handler);
        void ImportPressed(event_token const& token) noexcept;

        event_token ExportPressed(MainApplication::SingleVoidDelegate const& handler);
        void ExportPressed(event_token const& token) noexcept;

        event_token SortingChanged(MainApplication::SingleVoidDelegate const& handler);
        void SortingChanged(event_token const& token) noexcept;

    private:
        event<MainApplication::SingleVoidDelegate> m_import_pressed;
        event<MainApplication::SingleVoidDelegate> m_export_pressed;
        event<MainApplication::SingleVoidDelegate> m_sorting_changed;        
    };
}

namespace winrt::MainApplication::factory_implementation
{
    struct LoginDataListMenu : LoginDataListMenuT<LoginDataListMenu, implementation::LoginDataListMenu>
    {
    };
}
