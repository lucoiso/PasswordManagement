// Author: Lucas Oliveira Vilas-Bôas
// Year: 2023
// Repository: https://github.com/lucoiso/PasswordManagement

#pragma once

#ifndef LUPASS_FILELOADING_HELPER_H
#define LUPASS_FILELOADING_HELPER_H

#include "pch.h"

#include <ShObjIdl_core.h>
#include <App.xaml.h>

using namespace winrt::MainApplication;

namespace winrt::Helper
{
    inline Windows::Foundation::IAsyncOperation<Windows::Foundation::Collections::IVectorView<Windows::Storage::StorageFile>> LoadPasswordDataFilesAsync()
    {
        LUPASS_LOG_FUNCTION();

        Windows::Storage::Pickers::FileOpenPicker file_picker;
        file_picker.as<IInitializeWithWindow>()->Initialize(MainApplication::implementation::App::GetCurrentWindowHandle());

        file_picker.FileTypeFilter().Append(L".txt");
        file_picker.FileTypeFilter().Append(L".csv");
        file_picker.FileTypeFilter().Append(L".bin");

        return file_picker.PickMultipleFilesAsync();
    }

    inline Windows::Foundation::IAsyncOperation<Windows::Storage::StorageFile> SavePasswordDataFileAsync(const PasswordManager::LoginDataExportType& export_mode)
    {
        LUPASS_LOG_FUNCTION();

        Windows::Storage::Pickers::FileSavePicker file_picker;
        file_picker.as<IInitializeWithWindow>()->Initialize(MainApplication::implementation::App::GetCurrentWindowHandle());

        auto file_types = single_threaded_vector<hstring>();

        switch (export_mode)
        {
            case PasswordManager::LoginDataExportType::Lupass:
                file_types.Append(L".bin");
                break;

            case PasswordManager::LoginDataExportType::Microsoft:
            case PasswordManager::LoginDataExportType::Google:
            case PasswordManager::LoginDataExportType::Firefox:
                file_types.Append(L".csv");
                break;

            case PasswordManager::LoginDataExportType::Kapersky:
                file_types.Append(L".txt");
                break;

            default:
                throw hresult_invalid_argument(L"Invalid data type");
                break;
        }

        file_picker.FileTypeChoices().Insert(L"Password Data", file_types);

        return file_picker.PickSaveFileAsync();
    }

    inline Windows::Foundation::IAsyncOperation<Windows::Storage::StorageFile> GetLocalDataFileAsync()
    {
        LUPASS_LOG_FUNCTION();

        try
        {
            const Windows::Storage::StorageFolder localAppDir = Windows::Storage::ApplicationData::Current().LocalFolder();
            Helper::PrintDebugLine(localAppDir.Path().c_str());

            if (const auto existingFile = co_await localAppDir.TryGetItemAsync(L"data.bin"))
            {
                co_return co_await localAppDir.GetFileAsync(L"data.bin");
            }

            co_return co_await localAppDir.CreateFileAsync(L"data.bin");
        }
        catch (const hresult_error& e)
        {
            Helper::PrintDebugLine(e.message());
        }

        co_return nullptr;
    }
}
#endif
