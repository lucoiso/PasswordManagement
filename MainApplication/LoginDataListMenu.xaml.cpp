// Author: Lucas Oliveira Vilas-Bôas
// Year: 2023
// Repository: https://github.com/lucoiso/PasswordManagement

#include "pch.h"

#include "LoginDataListMenu.xaml.h"
#include "LoginDataListMenu.g.cpp"

#include "LoginDataEditor.xaml.h"
#include "MainPage.xaml.h"

#include "SecurityHelper.h"
#include "DialogHelper.h"
#include "CastingHelper.h"

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MainApplication::implementation
{
    LoginDataListMenu::LoginDataListMenu()
    {
        InitializeComponent();
    }

    void LoginDataListMenu::BP_Import_Click([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] RoutedEventArgs const& args)
    {
        m_import_pressed();
    }

    Windows::Foundation::IAsyncAction LoginDataListMenu::BP_Export_Click([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] RoutedEventArgs const& args)
    {
        if (!CB_ExportMode().SelectedItem() || !(co_await Helper::RequestUserCredentials(XamlRoot())))
        {
            co_return;
        }

        m_export_pressed();
    }

    Windows::Foundation::IAsyncAction LoginDataListMenu::BP_Add_Click(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args)
    {
        MainApplication::LoginDataEditor editor;
        editor.XamlRoot(XamlRoot());

        switch ((co_await editor.ShowAsync()))
        {
            case Microsoft::UI::Xaml::Controls::ContentDialogResult::Primary:
            {
                if (editor.Data().HasEmptyData())
                {
                    co_await Helper::CreateContentDialog(XamlRoot(), L"Error", L"Registered data contains empty values.", false, true).ShowAsync();
                }
                else
                {
                    if (auto MainPage = Helper::GetParent<MainApplication::MainPage>(Parent()); MainPage)
                    {
                        MainPage.AddLoginData(editor.Data());
                    }
                }

				break;
			}

            default: co_return;
        }
    }

    void LoginDataListMenu::BP_Generator_Click([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] Microsoft::UI::Xaml::RoutedEventArgs const& args)
    {
        Helper::InvokeGeneratorDialog(XamlRoot());
    }

    Windows::Foundation::IAsyncAction LoginDataListMenu::BP_Clear_Click([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] Microsoft::UI::Xaml::RoutedEventArgs const& args)
    {
        if (!(co_await Helper::RequestUserCredentials(XamlRoot())))
        {
            co_return;
        }

        auto confirm_dialog = Helper::CreateContentDialog(XamlRoot(), L"Delete All Data", L"Confirm process?", true, true);

        switch ((co_await confirm_dialog.ShowAsync()))
        {
            case Microsoft::UI::Xaml::Controls::ContentDialogResult::Primary:
            {
                if (auto MainPage = Helper::GetParent<MainApplication::MainPage>(*this); MainPage)
                {
                    MainPage.RemoveAllLoginData();
                }
            }
        }
    }

    void MainApplication::implementation::LoginDataListMenu::GenericSorting_SelectionChanged([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] Controls::SelectionChangedEventArgs const& args)
    {
        m_sorting_changed();
    }

    PasswordManager::LoginDataFileType LoginDataListMenu::SelectedExportDataType()
    {
        if (!CB_ExportMode().SelectedItem())
        {
            Helper::CreateContentDialog(XamlRoot(), L"Error", L"There's no selected export mode.", false, true).ShowAsync();
            return PasswordManager::LoginDataFileType::Undefined;
        }

        const hstring selectedValue = unbox_value<hstring>(unbox_value<Controls::ComboBoxItem>(CB_ExportMode().SelectedItem()).Content());

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
        if (!CB_SortingMode().SelectedItem())
        {
            Helper::CreateContentDialog(XamlRoot(), L"Error", L"There's no selected sorting mode.", false, true).ShowAsync();
            return DataSortMode::Undefined;
        }

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
        if (!CB_SortingOrientation().SelectedItem())
        {
            Helper::CreateContentDialog(XamlRoot(), L"Error", L"There's no selected sorting orientation.", false, true).ShowAsync();
            return DataSortOrientation::Undefined;
        }

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

    event_token MainApplication::implementation::LoginDataListMenu::SortingChanged(MainApplication::SingleVoidDelegate const& handler)
    {
        return m_sorting_changed.add(handler);
    }

    void MainApplication::implementation::LoginDataListMenu::SortingChanged(event_token const& token) noexcept
    {
        m_sorting_changed.remove(token);
    }
}
