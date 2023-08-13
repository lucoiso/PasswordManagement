// Author: Lucas Oliveira Vilas-Bôas
// Year: 2023
// Repository: https://github.com/lucoiso/PasswordManagement

#include "pch.h"

#include "LoginDataListMenu.xaml.h"
#include "LoginDataListMenu.g.cpp"

#include "LoginDataEditor.xaml.h"

#include "DialogManager.h"
#include "DataManager.h"

#include "Helpers/SecurityHelper.h"

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MainApplication::implementation
{
    LoginDataListMenu::LoginDataListMenu()
    {
        InitializeComponent();
    }

    void LoginDataListMenu::ComponentLoaded([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] Microsoft::UI::Xaml::RoutedEventArgs const& args)
    {
        if (!Helper::GetSettingValue<bool>(LICENSING_ENABLED_KEY))
        {
            BP_Import().IsEnabled(false);
            BP_Insert().IsEnabled(false);
            BP_Generator().IsEnabled(false);
        }
    }

    Windows::Foundation::IAsyncAction LoginDataListMenu::BP_Import_Click([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] RoutedEventArgs const& args)
    {
        LUPASS_LOG_FUNCTION();

        if (!(co_await Helper::RequestUserCredentials()))
        {
            co_return;
        }

        const auto load_files = co_await DataManager::GetInstance().LoadPasswordDataFilesAsync();
        std::vector<Windows::Storage::StorageFile> files;

        for (const auto& file : load_files)
        {
            files.push_back(file);
        }

        co_await DataManager::GetInstance().ImportDataFromFileAsync(files, true);
    }

    Windows::Foundation::IAsyncAction LoginDataListMenu::BP_Export_Click(Windows::Foundation::IInspectable const& sender, [[maybe_unused]] RoutedEventArgs const& args)
    {
        LUPASS_LOG_FUNCTION();

        const auto element = sender.as<Microsoft::UI::Xaml::FrameworkElement>();

        if (!(co_await Helper::RequestUserCredentials()))
        {
            co_return;
        }

        const auto tag = element.Tag().as<hstring>();

        MainApplication::LoginDataFileType export_type = MainApplication::LoginDataFileType::Undefined;

        if (Helper::StringEquals(tag, L"Lupass"))
        {
            export_type = MainApplication::LoginDataFileType::Lupass_External;
        }
        else if (Helper::StringEquals(tag, L"Microsoft"))
        {
            export_type = MainApplication::LoginDataFileType::Microsoft;
        }
        else if (Helper::StringEquals(tag, L"Google"))
        {
            export_type = MainApplication::LoginDataFileType::Google;
        }
        else if (Helper::StringEquals(tag, L"Firefox"))
        {
            export_type = MainApplication::LoginDataFileType::Firefox;
        }
        else if (Helper::StringEquals(tag, L"Kapersky"))
        {
            export_type = MainApplication::LoginDataFileType::Kapersky;
        }

        co_await DataManager::GetInstance().ExportDataAsync(export_type);
    }

    Windows::Foundation::IAsyncAction LoginDataListMenu::BP_Insert_Click(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args)
    {
        LUPASS_LOG_FUNCTION();

        MainApplication::LoginDataEditor editor;
        editor.XamlRoot(DialogManager::GetInstance().GetMainWindowContentXamlRoot());
        editor.Title(box_value(L"Creator"));

        switch ((co_await editor.ShowAsync()))
        {
        case Microsoft::UI::Xaml::Controls::ContentDialogResult::Primary:
        {
            if (editor.Data().HasEmptyData())
            {
                co_await DialogManager::GetInstance().ShowDialogAsync(L"Error", L"Registered data contains empty values.", false, true);
            }
            else
            {
                co_await DataManager::GetInstance().InsertLoginDataAsync({ editor.Data() }, true);
            }

            break;
        }

        default: co_return;
        }
    }

    Windows::Foundation::IAsyncAction LoginDataListMenu::BP_Generator_Click([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] Microsoft::UI::Xaml::RoutedEventArgs const& args)
    {
        LUPASS_LOG_FUNCTION();

        co_await DialogManager::GetInstance().InvokeGeneratorDialogAsync();
    }

    Windows::Foundation::IAsyncAction LoginDataListMenu::BP_Clear_Click([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] Microsoft::UI::Xaml::RoutedEventArgs const& args)
    {
        LUPASS_LOG_FUNCTION();

        if (!(co_await Helper::RequestUserCredentials()))
        {
            co_return;
        }

        const auto result = co_await DialogManager::GetInstance().ShowDialogAsync(L"Delete All Data", L"Confirm process?", true, true, L"Confirm", L"Cancel");
        switch (result)
        {
        case Microsoft::UI::Xaml::Controls::ContentDialogResult::Primary:
        {
            co_await DataManager::GetInstance().ClearLoginDataAsync(true);
        }
        }
    }

    void MainApplication::implementation::LoginDataListMenu::GenericSorting_SelectionChanged([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] Controls::SelectionChangedEventArgs const& args)
    {
        LUPASS_LOG_FUNCTION();

        DataManager::GetInstance().SetSortingMode(SelectedSortingMode());
        DataManager::GetInstance().SetSortingOrientation(SelectedSortingOrientation());
    }

    DataSortMode LoginDataListMenu::SelectedSortingMode()
    {
        LUPASS_LOG_FUNCTION();

        if (!CB_SortingMode() || !CB_SortingMode().SelectedItem())
        {
            return DataSortMode::Undefined;
        }

        const hstring selectedValue = unbox_value<hstring>(unbox_value<Microsoft::UI::Xaml::Controls::ComboBoxItem>(CB_SortingMode().SelectedItem()).Content());

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
        else if (Helper::StringEquals(selectedValue, L"Time Created"))
        {
            return DataSortMode::Created;
        }
        else if (Helper::StringEquals(selectedValue, L"Time Used"))
        {
            return DataSortMode::Used;
        }
        else if (Helper::StringEquals(selectedValue, L"Time Changed"))
        {
            return DataSortMode::Changed;
        }

        return DataSortMode::Undefined;
    }

    DataSortOrientation LoginDataListMenu::SelectedSortingOrientation()
    {
        LUPASS_LOG_FUNCTION();

        if (!CB_SortingOrientation() || !CB_SortingOrientation().SelectedItem())
        {
            return DataSortOrientation::Undefined;
        }

        const hstring selectedValue = unbox_value<hstring>(unbox_value<Microsoft::UI::Xaml::Controls::ComboBoxItem>(CB_SortingOrientation().SelectedItem()).Content());

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
}