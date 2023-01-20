// Author: Lucas Oliveira Vilas-Bôas
// Year: 2022
// Repository: https://github.com/lucoiso/PasswordManagement

#include "pch.h"

#include "LoginDataListMenu.xaml.h"
#include "LoginDataListMenu.g.cpp"

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MainApplication::implementation
{
    LoginDataListMenu::LoginDataListMenu()
    {
        InitializeComponent();
    }

    void LoginDataListMenu::BP_Import_Click([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] Microsoft::UI::Xaml::RoutedEventArgs const& args)
    {
        m_import_pressed();
    }

    void LoginDataListMenu::BP_Export_Click([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] Microsoft::UI::Xaml::RoutedEventArgs const& args)
    {
        m_export_pressed();
    }

    PasswordManager::LoginDataFileType LoginDataListMenu::SelectedExportDataType()
    {
        const hstring selectedValue = unbox_value<hstring>(CB_ExportMode().SelectedItem());

        if (selectedValue == L".txt")
        {
            return PasswordManager::LoginDataFileType::TXT;
        }
        else if (selectedValue == L".csv")
        {
            return PasswordManager::LoginDataFileType::CSV;
        }
        else if (selectedValue == L".bin")
        {
            return PasswordManager::LoginDataFileType::BIN;
        }

        return PasswordManager::LoginDataFileType::Undefined;
    }

    DataSortMode LoginDataListMenu::SelectedSortingMode()
    {
        const hstring selectedValue = unbox_value<hstring>(CB_SortingMode().SelectedItem());

        if (selectedValue == L"Name")
        {
            return DataSortMode::Name;
        }
        else if (selectedValue == L"Url")
        {
            return DataSortMode::Url;
        }
        else if (selectedValue == L"Username")
        {
            return DataSortMode::Username;
        }

        return DataSortMode::Undefined;
    }

    DataSortOrientation LoginDataListMenu::SelectedSortingOrientation()
    {
        const hstring selectedValue = unbox_value<hstring>(CB_SortingOrientation().SelectedItem());

        if (selectedValue == L"Ascending")
        {
            return DataSortOrientation::Ascending;
        }
        else if (selectedValue == L"Descending")
        {
            return DataSortOrientation::Descending;
        }

        return DataSortOrientation::Undefined;
    }

    event_token LoginDataListMenu::ImportPressed(MainApplication::SingleVoidDelegate const& handler)
    {
        return m_import_pressed.add(handler);
    }

    void LoginDataListMenu::ImportPressed(event_token const& token) noexcept
    {
        m_import_pressed.remove(token);
    }

    event_token LoginDataListMenu::ExportPressed(MainApplication::SingleVoidDelegate const& handler)
    {
        return m_export_pressed.add(handler);
    }
    
    void LoginDataListMenu::ExportPressed(event_token const& token) noexcept
    {
        m_export_pressed.remove(token);
    }
}
