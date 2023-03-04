// Author: Lucas Oliveira Vilas-Bôas
// Year: 2023
// Repository: https://github.com/lucoiso/PasswordManagement

#include "pch.h"
#include "MainPage.xaml.h"
#include "MainPage.g.cpp"

#include "FileLoadingHelper.h"
#include "DialogHelper.h"

#include <Helper.h>

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MainApplication::implementation
{    
    MainPage::MainPage()
    {
        InitializeComponent();
    }

    void MainApplication::implementation::MainPage::LI_LoginData_Loaded([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] RoutedEventArgs const& args)
    {
        { // Event bindings
            BindDataUpdate();
            BindDataLoad();
            BindDataSorting();
        }

        LoadLocalDataAsync();
    }

    void MainPage::AddLoginData(PasswordManager::LoginData const& data)
    {
        LI_LoginData().InsertDataInList(data);
        SaveLocalDataAsync();
    }

    void MainPage::RemoveLoginData(PasswordManager::LoginData const& data)
    {
        LI_LoginData().RemoveDataFromList(data);
        SaveLocalDataAsync();
    }

    void MainPage::RemoveAllLoginData()
    {
        LI_LoginData().RemoveAllDataFromList();
		SaveLocalDataAsync();
    }

    Windows::Foundation::IAsyncAction MainPage::PerformDataImportAsync()
    {
        const auto data_files = co_await Helper::LoadPasswordDataFilesAsync();
        for (const auto& iterator : data_files)
        {
            if (!iterator)
            {
                continue;
            }

            co_await m_manager.ImportDataAsync(iterator);
        }

        if (data_files.Size() > 0)
        {
            SaveLocalDataAsync();
        }
    }

    Windows::Foundation::IAsyncAction MainPage::PerformDataExportAsync()
    {
        if (const auto exported_file = co_await Helper::SavePasswordDataFileAsync(MO_DataOptions().SelectedExportDataType()))
        {
            co_await m_manager.ExportDataAsync(exported_file, LI_LoginData().Data().GetView());
        }
    }

    Windows::Foundation::IAsyncAction MainPage::LoadLocalDataAsync()
    {
        Controls::ProgressBar progress_bar;
        progress_bar.IsIndeterminate(true);
        const auto loading_dialog = Helper::CreateContentDialog(Content().XamlRoot(), L"Loading...", progress_bar, false, false);

        loading_dialog.ShowAsync();
        {
            try
            {
                co_await m_manager.ImportDataAsync(co_await Helper::GetLocalDataFileAsync());
            }
            catch (const hresult_error& e)
            {
                Helper::PrintDebugLine(e.message());
            }
        }
        loading_dialog.Hide();

        co_return;
    }

    Windows::Foundation::IAsyncAction MainPage::SaveLocalDataAsync()
    {
        Controls::ProgressBar progress_bar;
        progress_bar.IsIndeterminate(true);
        const auto loading_dialog = Helper::CreateContentDialog(Content().XamlRoot(), L"Loading...", progress_bar, false, false);

        loading_dialog.ShowAsync();
        {
            try
            {
                co_await m_manager.ExportDataAsync(co_await Helper::GetLocalDataFileAsync(), LI_LoginData().Data().GetView());
            }
            catch (const hresult_error& e)
            {
                Helper::PrintDebugLine(e.message());
            }
        }
        loading_dialog.Hide();

        co_return;
    }

    void MainPage::BindDataUpdate()
    {
        const auto update_data_lambda = [this]([[maybe_unused]] const auto&, PasswordManager::LoginUpdateEventParams const& event_data)
        {
            LI_LoginData().InsertDataInList(event_data.Data());
        };

        m_data_update_token = m_manager.LoginDataUpdated(update_data_lambda);
    }

    void MainPage::BindDataLoad()
    {
        const auto import_data_lambda = [this]()
        {
            try
            {
                PerformDataImportAsync();
            }
            catch (const hresult_error& e)
            {
                Helper::PrintDebugLine(e.message());
            }
        };

		m_data_import_token = MO_DataOptions().ImportPressed(import_data_lambda);

        const auto export_data_lambda = [this]()
        {
            try
            {
                PerformDataExportAsync();
            }
            catch (const hresult_error& e)
            {
                Helper::PrintDebugLine(e.message());
            }
        };

        m_data_export_token = MO_DataOptions().ExportPressed(export_data_lambda);
    }

    void MainApplication::implementation::MainPage::BindDataSorting()
    {
        const auto sort_data_lambda = [this]()
        {
            LI_LoginData().Sort(MO_DataOptions().SelectedSortingMode(), MO_DataOptions().SelectedSortingOrientation());
        };

        m_data_sort_token = MO_DataOptions().SortingChanged(sort_data_lambda);
    }
}
