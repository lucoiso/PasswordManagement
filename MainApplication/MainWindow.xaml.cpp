// Author: Lucas Oliveira Vilas-Bôas
// Year: 2022
// Repository: https://github.com/lucoiso/PasswordManagement

#include "pch.h"

#include "MainWindow.xaml.h"
#include "MainWindow.g.cpp"

#include <Microsoft.UI.Xaml.Window.h>
#include <ShObjIdl_core.h>

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MainApplication::implementation
{
    MainWindow::MainWindow()
    {
        InitializeComponent();

        InitializeSubobjects();
    }

    void MainWindow::InitializeSubobjects()
    {
        const auto update_data_lambda = [this]([[maybe_unused]] const auto&, PasswordManager::LoginUpdateEventParams const& event_data)
        {
            LI_LoginData().insertDataInList(event_data.Data());
        };

        this->m_data_update_token = this->m_manager.LoginDataUpdated(update_data_lambda);

        loadLocalData();
    }

    HWND MainWindow::getWindowHandle() const
    {
        auto windowNative{ this->try_as<IWindowNative>() };
        check_bool(windowNative);
        HWND hWnd{ 0 };
        windowNative->get_WindowHandle(&hWnd);

        return hWnd;
    }

    void MainWindow::BP_Import_Click([[maybe_unused]] winrt::Windows::Foundation::IInspectable const& sender, [[maybe_unused]] winrt::Microsoft::UI::Xaml::RoutedEventArgs const& args)
    {
        try
        {
            performDataImport();
        }
        catch (const hresult_error& e)
        {
            OutputDebugStringW(e.message().c_str());
        }
    }

    void MainWindow::BP_Export_Click([[maybe_unused]] winrt::Windows::Foundation::IInspectable const& sender, [[maybe_unused]] winrt::Microsoft::UI::Xaml::RoutedEventArgs const& args)
    {
        try
        {
            performDataExport();
        }
        catch (const hresult_error& e)
        {
            OutputDebugStringW(e.message().c_str());
        }
    }

    winrt::Windows::Foundation::IAsyncAction MainWindow::performDataImport()
    {
        for (const auto& iterator : co_await chooseFileInput())
        {
            if (!iterator)
            {
                continue;
            }

            if (iterator.FileType() == L".txt")
            {
                this->m_manager.importData(iterator);
            }
            else if (iterator.FileType() == L".csv")
            {
                this->m_manager.importData(iterator);
            }
        }

        saveLocalData();
    }

    winrt::Windows::Foundation::IAsyncOperation<winrt::Windows::Foundation::Collections::IVectorView<winrt::Windows::Storage::StorageFile>> MainWindow::chooseFileInput()
    {
        winrt::Windows::Storage::Pickers::FileOpenPicker file_picker;
        file_picker.as<IInitializeWithWindow>()->Initialize(getWindowHandle());

        file_picker.FileTypeFilter().Append(L".txt");
        file_picker.FileTypeFilter().Append(L".csv");

        return file_picker.PickMultipleFilesAsync();
    }

    winrt::Windows::Foundation::IAsyncAction MainWindow::performDataExport()
    {
        if (const auto exported_file = co_await chooseFileDestination())
        {
            this->m_manager.exportData(exported_file, LI_LoginData().Data().GetView());
        }
    }

    winrt::Windows::Foundation::IAsyncOperation<winrt::Windows::Storage::StorageFile> MainWindow::chooseFileDestination()
    {
        winrt::Windows::Storage::Pickers::FileSavePicker file_picker;
        file_picker.as<IInitializeWithWindow>()->Initialize(getWindowHandle());

        auto file_types = single_threaded_vector<hstring>();

        switch (getSelectedExportDataType())
        {
            case winrt::PasswordManager::LoginDataFileType::CSV:
                file_types.Append(L".csv");
                break;

            case winrt::PasswordManager::LoginDataFileType::TXT:
                file_types.Append(L".txt");
                break;

            default:
                throw hresult_not_implemented(L"not implemented yet");
                break;
        }

        file_picker.FileTypeChoices().Insert(L"Password Data", file_types);

        return file_picker.PickSaveFileAsync();
    }

    winrt::PasswordManager::LoginDataFileType MainWindow::getSelectedExportDataType()
    {
        const hstring selectedValue = CB_ExportMode().SelectedOption();

        if (selectedValue == L"Kapersky")
        {
            return winrt::PasswordManager::LoginDataFileType::TXT;
        }
        else if (selectedValue == L"Google" || selectedValue == L"Microsoft")
        {
            return winrt::PasswordManager::LoginDataFileType::CSV;
        }

        return winrt::PasswordManager::LoginDataFileType::Undefined;
    }

    winrt::Windows::Foundation::IAsyncOperation<winrt::Windows::Storage::StorageFile> MainWindow::getLocalDataFile() const
    {
        try
        {
            const winrt::Windows::Storage::StorageFolder localAppDir = winrt::Windows::Storage::ApplicationData::Current().LocalFolder();
            
            if (const auto existingFile = co_await localAppDir.TryGetItemAsync(L"app_data.csv"))
            {
                co_return co_await localAppDir.GetFileAsync(L"app_data.csv");
            }

            co_return co_await localAppDir.CreateFileAsync(L"app_data.csv");
        }
        catch (const hresult_error& e)
        {
            OutputDebugStringW(e.message().c_str());
        }

        co_return nullptr;
    }

    winrt::Windows::Foundation::IAsyncAction MainWindow::loadLocalData()
    {
        try
        {
            this->m_manager.importData(co_await getLocalDataFile());
        }
        catch (const hresult_error& e)
        {
            OutputDebugStringW(e.message().c_str());
        }

        co_return;
    }
    
    winrt::Windows::Foundation::IAsyncAction MainWindow::saveLocalData()
    {
        try
        {
            this->m_manager.exportData(co_await getLocalDataFile(), LI_LoginData().Data().GetView());
        }
        catch (const hresult_error& e)
        {
            OutputDebugStringW(e.message().c_str());
        }

        co_return;
    }
}
