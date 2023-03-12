// Author: Lucas Oliveira Vilas-Bôas
// Year: 2023
// Repository: https://github.com/lucoiso/PasswordManagement

#include "pch.h"

#include "DataManager.h"
#include "DialogManager.h"

#include "App.xaml.h"

#include "Helpers/SettingsHelper.h"
#include "Helpers/CastingHelper.h"

#include <ShObjIdl_core.h>
#include <Helper.h>

Windows::Foundation::IAsyncAction DataManager::Activate()
{
    if (m_activated)
    {
        return;
    }

    m_data_update_token = m_manager.LoginDataUpdated(
        [this]([[maybe_unused]] const auto&, PasswordManager::LoginUpdateEventParams const& event_data) -> Windows::Foundation::IAsyncAction
        {
            co_await InsertLoginDataAsync({ event_data.Data() }, false);
        }
    );
    
    co_await ImportDataFromFileAsync({ co_await GetLocalDataFileAsync() }, true);
    m_activated = true;
}

void DataManager::NotifyDataChange()
{
    m_data_synchronized_event();
}

void DataManager::SetSearchText(hstring const& value)
{
    if (Helper::SetMemberValue(value, m_search_text))
    {
        NotifyDataChange();
    }
}

void DataManager::SetSortingMode(DataSortMode const& value)
{
    if (Helper::SetMemberValue(value, m_sorting_mode))
    {
        NotifyDataChange();
    }
}

void DataManager::SetSortingOrientation(DataSortOrientation const& value)
{
    if (Helper::SetMemberValue(value, m_sorting_orientation))
    {
        NotifyDataChange();
    }
}

Windows::Foundation::Collections::IVectorView<PasswordManager::LoginData> DataManager::Data() const
{
    std::vector<PasswordManager::LoginData> filtered_container(m_data.size());

    std::copy_if(m_data.begin(), m_data.end(), filtered_container.begin(),
        [this](const PasswordManager::LoginData& iterator)
        {
            return m_search_text.empty()
                || Helper::StringContains(iterator.Name(), m_search_text)
                || Helper::StringContains(iterator.Url(), m_search_text)
                || Helper::StringContains(iterator.Username(), m_search_text)
                || Helper::StringContains(iterator.Notes(), m_search_text);
        }
    );

    const auto first_empty_iterator = std::find_if(filtered_container.begin(), filtered_container.end(), [](const PasswordManager::LoginData& iterator) { return iterator.HasEmptyData(); });
    filtered_container.erase(first_empty_iterator, filtered_container.end());

    std::sort(filtered_container.begin(), filtered_container.end(),
        [this](const PasswordManager::LoginData& lhs, const PasswordManager::LoginData& rhs)
        {
            const auto filter_orientation = [&](const auto& left_string, const auto& right_string) -> bool
            {
                switch (m_sorting_orientation)
                {
                    case DataSortOrientation::Ascending:
                        return left_string < right_string;

                    case DataSortOrientation::Descending:
                        return left_string > right_string;

                    default:
                        return false;
                }
            };

            switch (m_sorting_mode)
            {
                case DataSortMode::Name:
                    return filter_orientation(lhs.Name(), rhs.Name());

                case DataSortMode::Url:
                    return filter_orientation(lhs.Url(), rhs.Url());

                case DataSortMode::Username:
                    return filter_orientation(lhs.Username(), rhs.Username());

                case DataSortMode::Notes:
                    return filter_orientation(lhs.Notes(), rhs.Notes());

                case DataSortMode::Created:
                    return filter_orientation(lhs.Created(), rhs.Created());

                case DataSortMode::Used:
                    return filter_orientation(lhs.Used(), rhs.Used());

                case DataSortMode::Changed:
                    return filter_orientation(lhs.Changed(), rhs.Changed());

                default:
                    return false;
            }
        }
    );

    filtered_container.shrink_to_fit();

    const auto output = single_threaded_vector(std::move(filtered_container));
    return output.GetView();
}

uint32_t DataManager::DataSize() const
{
    return static_cast<uint32_t>(m_data.size());
}

Windows::Foundation::Collections::IVectorView<uint64_t> DataManager::Backups() const
{
    std::vector<uint64_t> copied_container(m_backups.size());
    std::copy(m_backups.begin(), m_backups.end(), copied_container.begin());

    copied_container.shrink_to_fit();

    const auto output = single_threaded_vector(std::move(copied_container));
    return output.GetView();
}

Windows::Foundation::IAsyncAction DataManager::InsertLoginDataAsync(const std::vector<PasswordManager::LoginData>& data, const bool save_data)
{
    for (const auto& iterator : data)
    {
        const PasswordManager::LoginData new_data = iterator.Clone().as<PasswordManager::LoginData>();
        new_data.InitializeInvalidTimes();

        const auto matching_iterator = std::find_if(m_data.begin(), m_data.end(), [new_data](const PasswordManager::LoginData& element) { return element.Equals(new_data); });
        if (matching_iterator == m_data.end())
        {
            m_data.push_back(new_data);
            continue;
        }

        const unsigned int index = static_cast<unsigned int>(std::distance(m_data.begin(), matching_iterator));
        if (new_data.Name().size() <= 0 || (Helper::GetCleanUrlString(new_data.Name()) == new_data.Url() && m_data.at(index).Name().size() > 0))
        {
            new_data.Name(m_data.at(index).Name());
        }

        m_data[index] = new_data;
    }

    if (save_data)
    {
        co_await SaveLocalDataFileAsync();
    }

    NotifyDataChange();
}

Windows::Foundation::IAsyncAction DataManager::RemoveLoginDataAsync(const std::vector<PasswordManager::LoginData>& data, const bool save_data)
{
    for (const auto& iterator : data)
    {
        const auto matching_iterator = std::find_if(m_data.begin(), m_data.end(), [iterator](const PasswordManager::LoginData& element) { return element.Equals(iterator); });
        if (matching_iterator == m_data.end())
        {
            continue;
        }

        m_data.erase(matching_iterator);
    }

    if (save_data)
    {
        co_await SaveLocalDataFileAsync();
    }

    NotifyDataChange();
}

Windows::Foundation::IAsyncAction DataManager::ClearLoginDataAsync(const bool save_data)
{
    m_data.clear();

    if (save_data)
    {
        co_await SaveLocalDataFileAsync();
    }

    NotifyDataChange();
}

Windows::Foundation::IAsyncAction DataManager::ImportDataFromFileAsync(const std::vector<Windows::Storage::StorageFile>& data, const bool save_data)
{
    LUPASS_LOG_FUNCTION();

    DialogManager::GetInstance().ShowLoadingDialog();

    try
    {
        for (const auto file : data)
        {
            PasswordManager::LoginDataExportType export_type = PasswordManager::LoginDataExportType::Undefined;
            const auto file_type = file.FileType();

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

            co_await m_manager.ImportDataAsync(file, export_type);
        }

        if (save_data)
        {
            co_await SaveLocalDataFileAsync();
        }
    }
    catch (const hresult_error& e)
    {
        DialogManager::GetInstance().HideLoadingDialog();
        Helper::PrintDebugLine(e.message());
    }

    DialogManager::GetInstance().HideLoadingDialog();
}

Windows::Foundation::IAsyncAction DataManager::ReplaceDataWithFileAsync(const std::vector<Windows::Storage::StorageFile>& data, const bool save_data)
{
    m_data.clear();
    co_await ImportDataFromFileAsync(data, save_data);
}

Windows::Foundation::IAsyncAction DataManager::ExportDataAsync(const PasswordManager::LoginDataExportType export_type) const
{
    LUPASS_LOG_FUNCTION();

    if (const auto file = co_await SavePasswordDataFileAsync(export_type))
    {
        DialogManager::GetInstance().ShowLoadingDialog();

        try
        {
            co_await m_manager.ExportDataAsync(file, Data(), export_type);
        }
        catch (const hresult_error& e)
        {
            DialogManager::GetInstance().HideLoadingDialog();
            Helper::PrintDebugLine(e.message());
        }

        DialogManager::GetInstance().HideLoadingDialog();
    }
}

Windows::Foundation::IAsyncOperation<Windows::Foundation::Collections::IVectorView<Windows::Storage::StorageFile>> DataManager::LoadPasswordDataFilesAsync() const
{
    LUPASS_LOG_FUNCTION();

    Windows::Storage::Pickers::FileOpenPicker file_picker;
    file_picker.as<IInitializeWithWindow>()->Initialize(MainApplication::implementation::App::GetCurrentWindowHandle());

    file_picker.FileTypeFilter().Append(L".txt");
    file_picker.FileTypeFilter().Append(L".csv");
    file_picker.FileTypeFilter().Append(L".bin");

    return file_picker.PickMultipleFilesAsync();
}

Windows::Foundation::IAsyncOperation<Windows::Storage::StorageFile> DataManager::SavePasswordDataFileAsync(const PasswordManager::LoginDataExportType& export_mode) const
{
    LUPASS_LOG_FUNCTION();

    Windows::Storage::Pickers::FileSavePicker file_picker;
    file_picker.as<IInitializeWithWindow>()->Initialize(MainApplication::implementation::App::GetCurrentWindowHandle());

    auto file_types = single_threaded_vector<hstring>();

    switch (export_mode)
    {
        case PasswordManager::LoginDataExportType::Lupass_Internal:
            file_types.Append(L".bin");
            break;

        case PasswordManager::LoginDataExportType::Lupass_External:
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

Windows::Foundation::IAsyncOperation<Windows::Storage::StorageFolder> DataManager::GetBackupsDirectoryAsync() const
{
    co_return co_await Windows::Storage::ApplicationData::Current().LocalFolder().CreateFolderAsync(APP_BACKUP_DATA_DIRECTORY_NAME, Windows::Storage::CreationCollisionOption::OpenIfExists);
}

Windows::Foundation::IAsyncOperation<Windows::Storage::StorageFile> DataManager::GetLocalDataFileAsync() const
{
    LUPASS_LOG_FUNCTION();

    try
    {
        const Windows::Storage::StorageFolder local_directory = Windows::Storage::ApplicationData::Current().LocalFolder();
        Helper::PrintDebugLine(hstring(L"Application data file: " + local_directory.Path()).c_str());

        if (const auto existing_file = co_await local_directory.TryGetItemAsync(APP_DATA_FILE_NAME); existing_file)
        {
            co_return co_await local_directory.GetFileAsync(APP_DATA_FILE_NAME);
        }

        co_return co_await local_directory.CreateFileAsync(APP_DATA_FILE_NAME);
    }
    catch (const hresult_error& e)
    {
        Helper::PrintDebugLine(e.message());
    }

    co_return nullptr;
}

Windows::Foundation::IAsyncAction DataManager::SaveLocalDataFileAsync() const
{
    try
    {
        co_await m_manager.ExportDataAsync(co_await GetLocalDataFileAsync(), Data(), PasswordManager::LoginDataExportType::Lupass_Internal);
    }
    catch (const hresult_error& e)
    {
        Helper::PrintDebugLine(e.message());
    }
}

Windows::Foundation::IAsyncAction DataManager::CopyCurrentDataAsBackupAsync() const
{
    LUPASS_LOG_FUNCTION();

    try
    {
        const Windows::Storage::StorageFolder backups_directory = co_await GetBackupsDirectoryAsync();
        const Windows::Storage::StorageFile local_data_file = co_await GetLocalDataFileAsync();

        const hstring file_name = to_hstring(Helper::GetCurrentDayTimeCount());

        const Windows::Storage::StorageFile backup_file = co_await backups_directory.CreateFileAsync(file_name, Windows::Storage::CreationCollisionOption::ReplaceExisting);
        co_await local_data_file.CopyAndReplaceAsync(backup_file);

        Helper::InsertSettingValue(file_name, Helper::GetSettingValue<hstring>(KEY_MATERIAL_TEMP_ID));
    }
    catch (const hresult_error& e)
    {
        Helper::PrintDebugLine(e.message());
    }
}

Windows::Foundation::IAsyncAction DataManager::RefreshBackupDataAsync()
{
    LUPASS_LOG_FUNCTION();

    std::vector<uint64_t> output;

    try
    {
        const Windows::Storage::StorageFolder backups_directory = co_await GetBackupsDirectoryAsync();
        const auto backup_files = co_await backups_directory.GetFilesAsync();

        const uint64_t current_day_count = Helper::GetCurrentDayTimeCount();
        bool contains_current_day = false;

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

            if (backup_time == current_day_count)
            {
                contains_current_day = true;
            }

            output.push_back(backup_time);
        }

        if (!contains_current_day)
        {
            co_await CopyCurrentDataAsBackupAsync();
            output.push_back(current_day_count);
        }

        std::sort(output.begin(), output.end(), std::greater<uint64_t>());

        constexpr unsigned short int max_backups = 5;
        if (output.size() > max_backups)
        {
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

    m_backups = output;
    NotifyDataChange();
}

event_token DataManager::DataSync(MainApplication::SingleVoidDelegate const& handler)
{
    return m_data_synchronized_event.add(handler);
}

void DataManager::DataSync(event_token const& token) noexcept
{
    m_data_synchronized_event.remove(token);
}
