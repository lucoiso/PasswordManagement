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

        void ComponentLoaded(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args);

        Windows::Foundation::IAsyncAction BP_Import_Click(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args);
        Windows::Foundation::IAsyncAction BP_Export_Click(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args);
        Windows::Foundation::IAsyncAction BP_Insert_Click(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args);
        Windows::Foundation::IAsyncAction BP_Generator_Click(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args);
        Windows::Foundation::IAsyncAction BP_Clear_Click(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args);

        void GenericSorting_SelectionChanged(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::Controls::SelectionChangedEventArgs const& args);

        MainApplication::DataSortMode SelectedSortingMode();
        MainApplication::DataSortOrientation SelectedSortingOrientation();
    };
}

namespace winrt::MainApplication::factory_implementation
{
    struct LoginDataListMenu : LoginDataListMenuT<LoginDataListMenu, implementation::LoginDataListMenu>
    {
    };
}
