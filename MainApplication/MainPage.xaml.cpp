// Author: Lucas Oliveira Vilas-Bôas
// Year: 2023
// Repository: https://github.com/lucoiso/PasswordManagement

#include "pch.h"
#include "MainPage.xaml.h"
#include "MainPage.g.cpp"

#include "App.xaml.h"

#include "DialogManager.h"

#include "Helpers/FileLoadingHelper.h"

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MainApplication::implementation
{    
    MainPage::MainPage()
    {
        InitializeComponent();

        auto main_window = Application::Current().as<winrt::MainApplication::implementation::App>()->Window();

        const auto ensure_save_lambda = [this, main_window]([[maybe_unused]] const auto& sender, [[maybe_unused]] const auto& args) -> Windows::Foundation::IAsyncAction
        {
            co_await SaveLocalDataAsync();
        };

        main_window.Closed(ensure_save_lambda);
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

    Windows::Foundation::IAsyncAction MainPage::InsertLoginData(PasswordManager::LoginData const& data, const bool save_data)
    {
        LUPASS_LOG_FUNCTION();

        DialogManager::GetInstance().ShowLoadingDialog();

        try
        {
            LI_LoginData().InsertDataInList(data);

            if (save_data)
            {
                co_await SaveLocalDataAsync();
            }
        }
        catch (const hresult_error& e)
        {
            DialogManager::GetInstance().HideLoadingDialog();
            Helper::PrintDebugLine(e.message());
        }

        DialogManager::GetInstance().HideLoadingDialog();
    }

    Windows::Foundation::IAsyncAction MainPage::RemoveLoginData(PasswordManager::LoginData const& data, const bool save_data)
    {
        LUPASS_LOG_FUNCTION();

        DialogManager::GetInstance().ShowLoadingDialog();

        try
        {
            LI_LoginData().RemoveDataFromList(data);

            if (save_data)
            {
                co_await SaveLocalDataAsync();
            }
        }
        catch (const hresult_error& e)
        {
            DialogManager::GetInstance().HideLoadingDialog();
            Helper::PrintDebugLine(e.message());
        }

        DialogManager::GetInstance().HideLoadingDialog();
    }

    Windows::Foundation::IAsyncAction MainPage::RemoveAllLoginData(const bool save_data)
    {
        LUPASS_LOG_FUNCTION();

        DialogManager::GetInstance().ShowLoadingDialog();

        try
        {
            LI_LoginData().RemoveAllDataFromList();

            if (save_data)
            {
                co_await SaveLocalDataAsync();
            }
        }
        catch (const hresult_error& e)
        {
            DialogManager::GetInstance().HideLoadingDialog();
            Helper::PrintDebugLine(e.message());
        }

        DialogManager::GetInstance().HideLoadingDialog();
    }

    Windows::Foundation::IAsyncAction MainPage::PerformDataImportAsync()
    {
        LUPASS_LOG_FUNCTION();

        const auto data_files = co_await Helper::LoadPasswordDataFilesAsync();
        if (data_files.Size() > 0)
        {
            DialogManager::GetInstance().ShowLoadingDialog();

            try
            {
                for (const auto& iterator : data_files)
                {
                    ImportDataFromFileAsync(iterator, false);
                }

                co_await SaveLocalDataAsync();
            }
            catch (const hresult_error& e)
            {
                DialogManager::GetInstance().HideLoadingDialog();
				Helper::PrintDebugLine(e.message());
			}

            DialogManager::GetInstance().HideLoadingDialog();
        }
    }

    Windows::Foundation::IAsyncAction MainPage::PerformDataExportAsync(const PasswordManager::LoginDataExportType export_type)
    {
        LUPASS_LOG_FUNCTION();

        if (const auto exported_file = co_await Helper::SavePasswordDataFileAsync(export_type))
        {
            DialogManager::GetInstance().ShowLoadingDialog();

            try
            {
                const auto registered_data = LI_LoginData().Data().GetView();
                co_await m_manager.ExportDataAsync(exported_file, registered_data, export_type);
            }
            catch (const hresult_error& e)
            {
                DialogManager::GetInstance().HideLoadingDialog();
                Helper::PrintDebugLine(e.message());
            }

            DialogManager::GetInstance().HideLoadingDialog();
        }
    }

    Windows::Foundation::IAsyncAction MainPage::LoadLocalDataAsync()
    {
        LUPASS_LOG_FUNCTION();

        DialogManager::GetInstance().ShowLoadingDialog();

        try
        {
            co_await m_manager.ImportDataAsync(co_await Helper::GetLocalDataFileAsync(), PasswordManager::LoginDataExportType::Lupass_Internal);
        }
        catch (const hresult_error& e)
        {
            DialogManager::GetInstance().HideLoadingDialog();
            Helper::PrintDebugLine(e.message());
        }

        DialogManager::GetInstance().HideLoadingDialog();
    }

    Windows::Foundation::IAsyncAction MainPage::SaveLocalDataAsync()
    {
        LUPASS_LOG_FUNCTION();

        DialogManager::GetInstance().ShowLoadingDialog();

        try
        {
            const auto registered_data = LI_LoginData().Data().GetView();
            co_await m_manager.ExportDataAsync(co_await Helper::GetLocalDataFileAsync(), registered_data, PasswordManager::LoginDataExportType::Lupass_Internal);

            co_await Helper::CopyDataAsBackup();
        }
        catch (const hresult_error& e)
        {
            DialogManager::GetInstance().HideLoadingDialog();
            Helper::PrintDebugLine(e.message());
        }

        DialogManager::GetInstance().HideLoadingDialog();
    }

    Windows::Foundation::IAsyncAction MainPage::ImportDataFromFileAsync(const Windows::Storage::StorageFile& data_file, const bool save_data)
    {
        LUPASS_LOG_FUNCTION();

        DialogManager::GetInstance().ShowLoadingDialog();

        try
        {
            PasswordManager::LoginDataExportType export_type = PasswordManager::LoginDataExportType::Undefined;
            const auto file_type = data_file.FileType();

            if (file_type == L".")
            {
                export_type = PasswordManager::LoginDataExportType::Lupass_Internal;
            }
            else if (file_type == L".csv")
            {
                export_type = PasswordManager::LoginDataExportType::GenericCSV;
            }
            else if (file_type == L".txt")
            {
                export_type = PasswordManager::LoginDataExportType::Kapersky;
            }

            co_await m_manager.ImportDataAsync(data_file, export_type);

            if (save_data)
            {
                co_await SaveLocalDataAsync();
            }
        }
        catch (const hresult_error& e)
        {
            DialogManager::GetInstance().HideLoadingDialog();
            Helper::PrintDebugLine(e.message());
        }

        DialogManager::GetInstance().HideLoadingDialog();
    }

    Windows::Foundation::IAsyncAction MainPage::ReplaceDataWithFileAsync(const Windows::Storage::StorageFile& data_file, const bool save_data)
    {
        LUPASS_LOG_FUNCTION();

        DialogManager::GetInstance().ShowLoadingDialog();

        try
        {
            co_await RemoveAllLoginData(false);
            co_await ImportDataFromFileAsync(data_file, save_data);
        }
        catch (const hresult_error& e)
        {
            DialogManager::GetInstance().HideLoadingDialog();
            Helper::PrintDebugLine(e.message());
        }

        DialogManager::GetInstance().HideLoadingDialog();
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

        const auto export_data_lambda = [this](const PasswordManager::LoginDataExportType export_type)
        {
            try
            {
                PerformDataExportAsync(export_type);
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
