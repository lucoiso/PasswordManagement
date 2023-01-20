// Author: Lucas Oliveira Vilas-Bôas
// Year: 2022
// Repository: https://github.com/lucoiso/PasswordManagement

#pragma once

#include "pch.h"

#include <ShObjIdl_core.h>
#include <App.xaml.h>

#include <Helper.h>

using namespace winrt::MainApplication;

namespace winrt::Helper
{
    Windows::Foundation::IAsyncOperation<Windows::Foundation::Collections::IVectorView<Windows::Storage::StorageFile>> loadPasswordDataFiles()
    {
        Windows::Storage::Pickers::FileOpenPicker file_picker;
        file_picker.as<IInitializeWithWindow>()->Initialize(MainApplication::implementation::App::getWindowHandle());

        file_picker.FileTypeFilter().Append(L".txt");
        file_picker.FileTypeFilter().Append(L".csv");
        file_picker.FileTypeFilter().Append(L".bin");

        return file_picker.PickMultipleFilesAsync();
    }

    Windows::Foundation::IAsyncOperation<Windows::Storage::StorageFile> savePasswordDataFile(const PasswordManager::LoginDataFileType& export_mode)
    {
        Windows::Storage::Pickers::FileSavePicker file_picker;
        file_picker.as<IInitializeWithWindow>()->Initialize(MainApplication::implementation::App::getWindowHandle());

        auto file_types = single_threaded_vector<hstring>();

        switch (export_mode)
        {
            case PasswordManager::LoginDataFileType::CSV:
                file_types.Append(L".csv");
                break;

            case PasswordManager::LoginDataFileType::TXT:
                file_types.Append(L".txt");
                break;

            case PasswordManager::LoginDataFileType::BIN:
                file_types.Append(L".bin");
                break;

            default:
                throw hresult_not_implemented(L"not implemented yet");
                break;
        }

        file_picker.FileTypeChoices().Insert(L"Password Data", file_types);

        return file_picker.PickSaveFileAsync();
    }

    Windows::Foundation::IAsyncOperation<Windows::Storage::StorageFile> getLocalDataFile()
    {
        try
        {
            const Windows::Storage::StorageFolder localAppDir = Windows::Storage::ApplicationData::Current().LocalFolder();
            Helper::printDebugLine(localAppDir.Path().c_str());

            if (const auto existingFile = co_await localAppDir.TryGetItemAsync(L"data.bin"))
            {
                co_return co_await localAppDir.GetFileAsync(L"data.bin");
            }

            co_return co_await localAppDir.CreateFileAsync(L"data.bin");
        }
        catch (const hresult_error& e)
        {
            Helper::printDebugLine(e.message());
        }

        co_return nullptr;
    }
}