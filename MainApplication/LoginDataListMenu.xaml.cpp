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

    bool LoginDataListMenu::EnableLicenseTools() const
    {
        return m_enable_license_tools;
    }

    void LoginDataListMenu::EnableLicenseTools(bool value)
    {
        Helper::SetMemberValue(value, m_enable_license_tools);
    }

    void LoginDataListMenu::BP_Import_Click([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] RoutedEventArgs const& args)
    {
        LUPASS_LOG_FUNCTION();

        m_import_pressed();
    }

    Windows::Foundation::IAsyncAction LoginDataListMenu::BP_Export_Click(Windows::Foundation::IInspectable const& sender, [[maybe_unused]] RoutedEventArgs const& args)
    {
        LUPASS_LOG_FUNCTION();

        const auto element = sender.as<Microsoft::UI::Xaml::FrameworkElement>();

        if (!(co_await Helper::RequestUserCredentials(XamlRoot())))
        {
            co_return;
        }

        const auto tag = element.Tag().as<hstring>();

        if (Helper::StringEquals(tag, L"Lupass"))
        {
            m_export_pressed(PasswordManager::LoginDataExportType::Lupass);
        }
        else if (Helper::StringEquals(tag, L"Microsoft"))
        {
            m_export_pressed(PasswordManager::LoginDataExportType::Microsoft);
        }
        else if (Helper::StringEquals(tag, L"Google"))
        {
            m_export_pressed(PasswordManager::LoginDataExportType::Google);
        }
        else if (Helper::StringEquals(tag, L"Firefox"))
        {
            m_export_pressed(PasswordManager::LoginDataExportType::Firefox);
        }
        else if (Helper::StringEquals(tag, L"Kapersky"))
        {
            m_export_pressed(PasswordManager::LoginDataExportType::Kapersky);
        }
    }

    Windows::Foundation::IAsyncAction LoginDataListMenu::BP_Insert_Click(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args)
    {
        LUPASS_LOG_FUNCTION();

        MainApplication::LoginDataEditor editor;
        editor.XamlRoot(XamlRoot());
        editor.Title(box_value(L"Creator"));

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
                        co_await MainPage.InsertLoginData(editor.Data(), true);
                    }
                }

				break;
			}

            default: co_return;
        }
    }

    void LoginDataListMenu::BP_Generator_Click([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] Microsoft::UI::Xaml::RoutedEventArgs const& args)
    {
        LUPASS_LOG_FUNCTION();

        Helper::InvokeGeneratorDialog(XamlRoot());
    }

    Windows::Foundation::IAsyncAction LoginDataListMenu::BP_Clear_Click([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] Microsoft::UI::Xaml::RoutedEventArgs const& args)
    {
        LUPASS_LOG_FUNCTION();

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
                    confirm_dialog.Hide();
                    co_await MainPage.RemoveAllLoginData(true);
                }
            }
        }
    }

    void MainApplication::implementation::LoginDataListMenu::GenericSorting_SelectionChanged([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] Controls::SelectionChangedEventArgs const& args)
    {
        LUPASS_LOG_FUNCTION();

        m_sorting_changed();
    }

    DataSortMode LoginDataListMenu::SelectedSortingMode()
    {
        LUPASS_LOG_FUNCTION();

        if (!CB_SortingMode().SelectedItem())
        {
            Helper::CreateContentDialog(XamlRoot(), L"Error", L"Invalid sorting mode.", false, true).ShowAsync();
            return DataSortMode::Undefined;
        }

        const hstring selectedValue = unbox_value<hstring>(CB_SortingMode().SelectedItem());

        if (Helper::StringEquals(selectedValue, L"Name"))
        {
            return DataSortMode::Name;
        }
        else if (Helper::StringEquals(selectedValue, L"Url"))
        {
            return DataSortMode::Url;
        }
        else if (Helper::StringEquals(selectedValue, L"Username"))
        {
            return DataSortMode::Username;
        }
        else if (Helper::StringEquals(selectedValue, L"Notes"))
        {
            return DataSortMode::Notes;
        }

        return DataSortMode::Undefined;
    }

    DataSortOrientation LoginDataListMenu::SelectedSortingOrientation()
    {
        LUPASS_LOG_FUNCTION();

        if (!CB_SortingOrientation().SelectedItem())
        {
            Helper::CreateContentDialog(XamlRoot(), L"Error", L"Invalid sorting orientation.", false, true).ShowAsync();
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
        LUPASS_LOG_FUNCTION();

        return m_import_pressed.add(handler);
    }

    void LoginDataListMenu::ImportPressed(event_token const& token) noexcept
    {
        LUPASS_LOG_FUNCTION();

        m_import_pressed.remove(token);
    }

    event_token LoginDataListMenu::ExportPressed(MainApplication::ExportTypeDelegate const& handler)
    {
        return m_export_pressed.add(handler);
    }
    
    void LoginDataListMenu::ExportPressed(event_token const& token) noexcept
    {
        LUPASS_LOG_FUNCTION();

        m_export_pressed.remove(token);
    }

    event_token MainApplication::implementation::LoginDataListMenu::SortingChanged(MainApplication::SingleVoidDelegate const& handler)
    {
        LUPASS_LOG_FUNCTION();

        return m_sorting_changed.add(handler);
    }

    void MainApplication::implementation::LoginDataListMenu::SortingChanged(event_token const& token) noexcept
    {
        LUPASS_LOG_FUNCTION();

        m_sorting_changed.remove(token);
    }
}
