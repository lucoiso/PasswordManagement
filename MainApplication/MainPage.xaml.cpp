// Author: Lucas Oliveira Vilas-Bôas
// Year: 2022
// Repository: https://github.com/lucoiso/PasswordManagement

#include "pch.h"
#include "MainPage.xaml.h"
#include "MainPage.g.cpp"

#include "FileLoadingHelper.h"

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

    Windows::Foundation::IAsyncAction MainPage::PerformDataImportAsync()
    {
        for (const auto& iterator : co_await Helper::LoadPasswordDataFilesAsync())
        {
            if (!iterator)
            {
                continue;
            }

            m_manager.ImportDataAsync(iterator);
        }

        SaveLocalDataAsync();
    }

    Windows::Foundation::IAsyncAction MainPage::PerformDataExportAsync()
    {
        if (const auto exported_file = co_await Helper::SavePasswordDataFileAsync(MO_DataOptions().SelectedExportDataType()))
        {
            m_manager.ExportDataAsync(exported_file, LI_LoginData().Data().GetView());
        }
    }

    Windows::Foundation::IAsyncAction MainPage::LoadLocalDataAsync()
    {
        try
        {
            m_manager.ImportDataAsync(co_await Helper::GetLocalDataFileAsync());
        }
        catch (const hresult_error& e)
        {
            Helper::PrintDebugLine(e.message());
        }

        co_return;
    }

    Windows::Foundation::IAsyncAction MainPage::SaveLocalDataAsync()
    {
        try
        {
            m_manager.ExportDataAsync(co_await Helper::GetLocalDataFileAsync(), LI_LoginData().Data().GetView());
        }
        catch (const hresult_error& e)
        {
            Helper::PrintDebugLine(e.message());
        }

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
