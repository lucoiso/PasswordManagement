// Author: Lucas Oliveira Vilas-Bôas
// Year: 2022
// Repository: https://github.com/lucoiso/PasswordManagement

#pragma once

#include "pch.h"

#include <ShObjIdl_core.h>
#include <App.xaml.h>

#include <Helper.h>

using namespace winrt::MainApplication;

namespace Helper
{
    winrt::Windows::Foundation::IAsyncOperation<winrt::Windows::Foundation::Collections::IVectorView<winrt::Windows::Storage::StorageFile>> chooseFileInput()
    {
        winrt::Windows::Storage::Pickers::FileOpenPicker file_picker;
        file_picker.as<IInitializeWithWindow>()->Initialize(winrt::MainApplication::implementation::App::getWindowHandle());

        file_picker.FileTypeFilter().Append(L".txt");
        file_picker.FileTypeFilter().Append(L".csv");

        return file_picker.PickMultipleFilesAsync();
    }

    winrt::Windows::Foundation::IAsyncOperation<winrt::Windows::Storage::StorageFile> chooseFileDestination(const winrt::PasswordManager::LoginDataFileType& export_mode)
    {
        winrt::Windows::Storage::Pickers::FileSavePicker file_picker;
        file_picker.as<IInitializeWithWindow>()->Initialize(winrt::MainApplication::implementation::App::getWindowHandle());

        auto file_types = winrt::single_threaded_vector<winrt::hstring>();

        switch (export_mode)
        {
            case winrt::PasswordManager::LoginDataFileType::CSV:
                file_types.Append(L".csv");
                break;

            case winrt::PasswordManager::LoginDataFileType::TXT:
                file_types.Append(L".txt");
                break;

            default:
                throw winrt::hresult_not_implemented(L"not implemented yet");
                break;
        }

        file_picker.FileTypeChoices().Insert(L"Password Data", file_types);

        return file_picker.PickSaveFileAsync();
    }

    winrt::Windows::Foundation::IAsyncOperation<winrt::Windows::Storage::StorageFile> getLocalDataFile()
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
        catch (const winrt::hresult_error& e)
        {
            Helper::printDebugLine(e.message());
        }

        co_return nullptr;
    }
}