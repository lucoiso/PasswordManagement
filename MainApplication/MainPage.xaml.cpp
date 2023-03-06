// Author: Lucas Oliveira Vilas-Bôas
// Year: 2023
// Repository: https://github.com/lucoiso/PasswordManagement

#include "pch.h"
#include "MainPage.xaml.h"
#include "MainPage.g.cpp"

#include "FileLoadingHelper.h"
#include "DialogHelper.h"

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MainApplication::implementation
{    
    MainPage::MainPage()
    {
        InitializeComponent();
    }

    bool MainPage::EnableLicenseTools() const
    {
        return m_enable_license_tools;
    }

    Windows::Foundation::IAsyncAction MainPage::OnNavigatedTo(Microsoft::UI::Xaml::Navigation::NavigationEventArgs const& e)
    {
        LUPASS_LOG_FUNCTION();

        m_enable_license_tools = unbox_value<bool>(e.Parameter());

        { // Event bindings
            BindDataUpdate();
            BindDataLoad();
            BindDataSorting();
        }

        co_return co_await LoadLocalDataAsync();
    }

    void MainPage::AddLoginData(PasswordManager::LoginData const& data)
    {
        LUPASS_LOG_FUNCTION();

        LI_LoginData().InsertDataInList(data);
        SaveLocalDataAsync();
    }

    void MainPage::RemoveLoginData(PasswordManager::LoginData const& data)
    {
        LUPASS_LOG_FUNCTION();

        LI_LoginData().RemoveDataFromList(data);
        SaveLocalDataAsync();
    }

    void MainPage::RemoveAllLoginData()
    {
        LUPASS_LOG_FUNCTION();

        LI_LoginData().RemoveAllDataFromList();
		SaveLocalDataAsync();
    }

    Windows::Foundation::IAsyncAction MainPage::PerformDataImportAsync()
    {
        LUPASS_LOG_FUNCTION();

        const auto data_files = co_await Helper::LoadPasswordDataFilesAsync();
        if (data_files.Size() > 0)
        {
            const auto loading_dialog = Helper::CreateLoadingDialog(XamlRoot());
            loading_dialog.ShowAsync();
            {
                for (const auto& iterator : data_files)
                {
                    if (!iterator)
                    {
                        continue;
                    }

                    co_await m_manager.ImportDataAsync(iterator);
                }

                co_await SaveLocalDataAsync();
            }
            loading_dialog.Hide();
        }
    }

    Windows::Foundation::IAsyncAction MainPage::PerformDataExportAsync()
    {
        LUPASS_LOG_FUNCTION();

        if (const auto exported_file = co_await Helper::SavePasswordDataFileAsync(MO_DataOptions().SelectedExportDataType()))
        {
            const auto loading_dialog = Helper::CreateLoadingDialog(XamlRoot());
            loading_dialog.ShowAsync();
            {
                co_await m_manager.ExportDataAsync(exported_file, LI_LoginData().Data().GetView());
            }
            loading_dialog.Hide();
        }
    }

    Windows::Foundation::IAsyncAction MainPage::LoadLocalDataAsync()
    {
        LUPASS_LOG_FUNCTION();

        try
        {
            co_await m_manager.ImportDataAsync(co_await Helper::GetLocalDataFileAsync());
        }
        catch (const hresult_error& e)
        {
            Helper::PrintDebugLine(e.message());
        }
    }

    Windows::Foundation::IAsyncAction MainPage::SaveLocalDataAsync()
    {
        LUPASS_LOG_FUNCTION();

        try
        {
            co_await m_manager.ExportDataAsync(co_await Helper::GetLocalDataFileAsync(), LI_LoginData().Data().GetView());
        }
        catch (const hresult_error& e)
        {
            Helper::PrintDebugLine(e.message());
        }
    }

    void MainPage::BindDataUpdate()
    {
        LUPASS_LOG_FUNCTION();

        const auto update_data_lambda = [this]([[maybe_unused]] const auto&, PasswordManager::LoginUpdateEventParams const& event_data)
        {
            LI_LoginData().InsertDataInList(event_data.Data());
        };

        m_data_update_token = m_manager.LoginDataUpdated(update_data_lambda);
    }

    void MainPage::BindDataLoad()
    {
        LUPASS_LOG_FUNCTION();

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
        LUPASS_LOG_FUNCTION();

        const auto sort_data_lambda = [this]()
        {
            LI_LoginData().Sort(MO_DataOptions().SelectedSortingMode(), MO_DataOptions().SelectedSortingOrientation());
        };

        m_data_sort_token = MO_DataOptions().SortingChanged(sort_data_lambda);
    }
}
