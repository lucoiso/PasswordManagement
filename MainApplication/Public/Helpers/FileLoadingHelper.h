// Author: Lucas Oliveira Vilas-Bôas
// Year: 2023
// Repository: https://github.com/lucoiso/PasswordManagement

#pragma once

#ifndef LUPASS_FILELOADING_HELPER_H
#define LUPASS_FILELOADING_HELPER_H

#include "pch.h"

#include <ShObjIdl_core.h>
#include <App.xaml.h>

#include "SettingsHelper.h"

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
            const Windows::Storage::StorageFolder local_directory = Windows::Storage::ApplicationData::Current().LocalFolder();
            Helper::PrintDebugLine(hstring(L"Application data file: " + local_directory.Path()).c_str());

            if (const auto existingFile = co_await local_directory.TryGetItemAsync(L"data.bin"))
            {
                co_return co_await local_directory.GetFileAsync(L"data.bin");
            }

            co_return co_await local_directory.CreateFileAsync(L"data.bin");
        }
        catch (const hresult_error& e)
        {
            Helper::PrintDebugLine(e.message());
        }

        co_return nullptr;
    }

    inline Windows::Foundation::IAsyncOperation<Windows::Foundation::Collections::IVectorView<uint64_t>> GetExistingBackups()
    {
        LUPASS_LOG_FUNCTION();

        std::vector<uint64_t> output;

        try
        {
            const Windows::Storage::StorageFolder backups_directory = co_await Windows::Storage::ApplicationData::Current().LocalFolder().CreateFolderAsync(L"backups", Windows::Storage::CreationCollisionOption::OpenIfExists);
            const auto backup_files = co_await backups_directory.GetFilesAsync();

            for (const auto& file : backup_files)
            {
                Helper::PrintDebugLine(hstring(L"File found in backup directory: " + file.Path()).c_str());

                if (!file)
                {
                    continue;
                }

                const std::string file_name = to_string(file.DisplayName());

                bool valid_file = !file_name.empty();
                for (const auto& character : file_name)
                {
                    if (!std::isdigit(character))
                    {
                        valid_file = false;
                        break;
                    }
                }

                if (!valid_file)
                {
                    co_await file.DeleteAsync();
                    continue;
                }

                const uint64_t backup_time = std::stoull(file_name);

                output.push_back(backup_time);
            }

            constexpr unsigned short int max_backups = 5;

            if (output.size() > max_backups)
            {
                std::sort(output.begin(), output.end(), std::greater<uint64_t>());

                for (uint32_t iterator = max_backups; iterator < output.size(); ++iterator)
                {
                    const uint64_t backup_time = output.at(iterator);
                    const hstring file_name = to_hstring(backup_time);
                    const Windows::Storage::StorageFile file = co_await backups_directory.GetFileAsync(file_name);

                    Helper::RemoveSettingKey(file_name);

                    co_await file.DeleteAsync();
                }

                output.resize(max_backups);
            }
        }
        catch (const hresult_error& e)
        {
            Helper::PrintDebugLine(e.message());
        }

        const auto output_foundation_vector = single_threaded_vector(std::move(output));
        co_return output_foundation_vector.GetView();
    }

    inline Windows::Foundation::IAsyncAction CopyDataAsBackup()
    {
        LUPASS_LOG_FUNCTION();

        try
        {
			const Windows::Storage::StorageFolder backups_directory = co_await Windows::Storage::ApplicationData::Current().LocalFolder().CreateFolderAsync(L"backups", Windows::Storage::CreationCollisionOption::OpenIfExists);
			const Windows::Storage::StorageFile local_data_file = co_await GetLocalDataFileAsync();

            const uint64_t backup_time = winrt::clock::now().time_since_epoch().count() / (static_cast<long long>(24 * 60 * 60) * 10000000) * (static_cast<long long>(24 * 60 * 60) * 10000000);;
			const hstring file_name = to_hstring(backup_time);

			const Windows::Storage::StorageFile backup_file = co_await backups_directory.CreateFileAsync(file_name, Windows::Storage::CreationCollisionOption::ReplaceExisting);
			co_await local_data_file.CopyAndReplaceAsync(backup_file);

			Helper::InsertSettingValue(file_name, Helper::GetSettingValue<hstring>(KEY_MATERIAL_TEMP_ID));
		}
        catch (const hresult_error& e)
        {
			Helper::PrintDebugLine(e.message());
		}
    }
}
#endif
