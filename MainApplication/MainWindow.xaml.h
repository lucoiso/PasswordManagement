// Author: Lucas Oliveira Vilas-Bôas
// Year: 2022
// Repository: https://github.com/lucoiso/PasswordManagement

#pragma once

#include "MainWindow.g.h"

namespace winrt::MainApplication::implementation
{
    struct MainWindow : MainWindowT<MainWindow>
    {
        MainWindow();

        void InitializeSubobjects();
        HWND getWindowHandle() const;

        void BP_Import_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& args);
        void BP_Export_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& args);

    protected:
        winrt::Windows::Foundation::IAsyncAction performDataImport();
        winrt::Windows::Foundation::IAsyncOperation<winrt::Windows::Foundation::Collections::IVectorView<winrt::Windows::Storage::StorageFile>> chooseFileInput();

        winrt::Windows::Foundation::IAsyncAction performDataExport();
        winrt::Windows::Foundation::IAsyncOperation<winrt::Windows::Storage::StorageFile> chooseFileDestination();

        winrt::PasswordManager::LoginDataFileType getSelectedExportDataType();

    private:
        winrt::PasswordManager::LoginDataManager m_manager;
        event_token m_data_update_token;

        winrt::Windows::Foundation::IAsyncOperation<winrt::Windows::Storage::StorageFile> getLocalDataFile() const;

        winrt::Windows::Foundation::IAsyncAction loadLocalData();
        winrt::Windows::Foundation::IAsyncAction saveLocalData();
    };
}

namespace winrt::MainApplication::factory_implementation
{
    struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow>
    {
    };
}
