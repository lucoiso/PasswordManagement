// Author: Lucas Oliveira Vilas-Bôas
// Year: 2023
// Repository: https://github.com/lucoiso/PasswordManagement

#include "pch.h"

#include "DataManager.h"
#include "DialogManager.h"

#include "App.xaml.h"

#include "Helpers/SettingsHelper.h"
#include "Helpers/CastingHelper.h"

#include "LoginData.h"

#include <ShObjIdl_core.h>

DataManager::DataManager()
    : m_sorting_mode(MainApplication::DataSortMode::Name)
    , m_sorting_orientation(MainApplication::DataSortOrientation::Ascending)
{
}

DataManager::~DataManager() = default;

Windows::Foundation::IAsyncAction DataManager::Activate()
{
    if (m_activated)
    {
        return;
    }

    co_await ImportDataFromFileAsync({ co_await GetLocalDataFileAsync() }, false);

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

Windows::Foundation::Collections::IVectorView<MainApplication::LoginData> DataManager::Data(const bool apply_filter) const
{
    if (!apply_filter)
    {
        std::vector<MainApplication::LoginData> copied_data = m_data;
        return single_threaded_vector(std::move(copied_data)).GetView();
    }

    std::vector<MainApplication::LoginData> filtered_container(m_data.size());

    std::copy_if(m_data.begin(), m_data.end(), filtered_container.begin(),
        [this](const MainApplication::LoginData& iterator)
        {
            return m_search_text.empty()
                || Helper::StringContains(iterator.Name(), m_search_text)
                || Helper::StringContains(iterator.Url(), m_search_text)
                || Helper::StringContains(iterator.Username(), m_search_text)
                || Helper::StringContains(iterator.Notes(), m_search_text);
        }
    );

    std::sort(filtered_container.begin(), filtered_container.end(),
        [this](const MainApplication::LoginData& lhs, const MainApplication::LoginData& rhs)
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

Windows::Foundation::IAsyncAction DataManager::InsertLoginDataAsync(const std::vector<MainApplication::LoginData>& data, const bool save_data)
{
    for (const auto& iterator : data)
    {
        iterator.InitializeInvalidTimes();

        const auto matching_iterator = std::find_if(m_data.begin(), m_data.end(), [iterator](const MainApplication::LoginData& element) { return element.Equals(iterator); });
        if (matching_iterator == m_data.end())
        {
            m_data.emplace_back(iterator);
            continue;
        }

        const unsigned int index = static_cast<unsigned int>(std::distance(m_data.begin(), matching_iterator));
        if (iterator.Name().size() <= 0 || (Helper::GetCleanUrlString(iterator.Name()) == iterator.Url() && m_data.at(index).Name().size() > 0))
        {
            iterator.Name(m_data.at(index).Name());
        }

        m_data[index] = iterator;
    }

    const auto invalid_data_it = std::remove_if(m_data.begin(), m_data.end(), [](MainApplication::LoginData const& iterator) { return iterator.HasEmptyData(); });
    m_data.erase(invalid_data_it, m_data.end());

    if (save_data)
    {
        co_await SaveLocalDataFileAsync();
    }

    NotifyDataChange();
}

Windows::Foundation::IAsyncAction DataManager::RemoveLoginDataAsync(const std::vector<MainApplication::LoginData>& data, const bool save_data)
{
    for (const auto& iterator : data)
    {
        const auto matching_iterator = std::find_if(m_data.begin(), m_data.end(), [iterator](const MainApplication::LoginData& element) { return element.Equals(iterator); });
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
        for (const auto& file : data)
        {
            MainApplication::LoginDataFileType export_type = MainApplication::LoginDataFileType::Undefined;
            const auto file_type = file.FileType();

            if (file_type == L".")
            {
                export_type = MainApplication::LoginDataFileType::Lupass_Internal;
            }
            else if (file_type == L".csv")
            {
                export_type = MainApplication::LoginDataFileType::GenericCSV;
            }
            else if (file_type == L".txt")
            {
                export_type = MainApplication::LoginDataFileType::Kapersky;
            }

            co_await ImportDataAsync(file, export_type);
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

Windows::Foundation::IAsyncAction DataManager::ExportDataAsync(const MainApplication::LoginDataFileType export_type) const
{
    LUPASS_LOG_FUNCTION();

    if (const auto file = co_await SavePasswordDataFileAsync(export_type))
    {
        DialogManager::GetInstance().ShowLoadingDialog();

        try
        {
            co_await ExportDataAsync(file, Data(), export_type);
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

Windows::Foundation::IAsyncOperation<Windows::Storage::StorageFile> DataManager::SavePasswordDataFileAsync(const MainApplication::LoginDataFileType& export_mode) const
{
    LUPASS_LOG_FUNCTION();

    Windows::Storage::Pickers::FileSavePicker file_picker;
    file_picker.as<IInitializeWithWindow>()->Initialize(MainApplication::implementation::App::GetCurrentWindowHandle());

    auto file_types = single_threaded_vector<hstring>();

    switch (export_mode)
    {
    case MainApplication::LoginDataFileType::Lupass_Internal:
        file_types.Append(L".bin");
        break;

    case MainApplication::LoginDataFileType::Lupass_External:
    case MainApplication::LoginDataFileType::Microsoft:
    case MainApplication::LoginDataFileType::Google:
    case MainApplication::LoginDataFileType::Firefox:
        file_types.Append(L".csv");
        break;

    case MainApplication::LoginDataFileType::Kapersky:
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
        if (const Windows::Storage::StorageFile local_data_file = co_await GetLocalDataFileAsync())
        {
            co_await ExportDataAsync(local_data_file, Data(), MainApplication::LoginDataFileType::Lupass_Internal);
        }
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

Windows::Foundation::IAsyncAction DataManager::ImportDataAsync(Windows::Storage::StorageFile const& file, const MainApplication::LoginDataFileType import_type)
{
    LUPASS_LOG_FUNCTION();

    if (!file.IsAvailable())
    {
        throw hresult_invalid_argument(L"file is unavailable");
    }

    if (const auto basic_properties = co_await file.GetBasicPropertiesAsync();
        basic_properties.Size() <= 0)
    {
        co_return;
    }

    std::vector<MainApplication::LoginData> output;

    if (import_type == MainApplication::LoginDataFileType::Lupass_Internal)
    {
        try
        {
            const hstring file_name = file.DisplayName();
            const auto data_buffer = co_await Windows::Storage::FileIO::ReadBufferAsync(file);
            for (const auto& import_result_it : co_await ReadData_Lupass(file_name, data_buffer))
            {
                output.emplace_back(import_result_it);
            }

            co_await PushData(output, import_type);
        }
        catch (const hresult_error& e)
        {
            Helper::PrintDebugLine(e.message());
        }

        co_return;
    }

    const auto fileContent = co_await Windows::Storage::FileIO::ReadLinesAsync(file);
    if (fileContent.Size() == 0)
    {
        co_return;
    }

    const auto fileContentView = fileContent.GetView();

    switch (import_type)
    {
    case MainApplication::LoginDataFileType::Lupass_External:
    case MainApplication::LoginDataFileType::Microsoft:
    case MainApplication::LoginDataFileType::Google:
    case MainApplication::LoginDataFileType::Firefox:
        output = ReadData_GenericCSV(fileContentView, import_type);
        break;

    case MainApplication::LoginDataFileType::Kapersky:
        output = ReadData_Kapersky(fileContentView);
        break;

    case MainApplication::LoginDataFileType::GenericCSV:
    {
        const hstring header = fileContentView.GetAt(0);

        if (header == PASSWORD_DATA_LUPASS_HEADER)
        {
            output = ReadData_GenericCSV(fileContentView, MainApplication::LoginDataFileType::Lupass_External);
        }
        else if (header == PASSWORD_DATA_MICROSOFT_HEADER)
        {
            output = ReadData_GenericCSV(fileContentView, MainApplication::LoginDataFileType::Microsoft);
        }
        else if (header == PASSWORD_DATA_GOOGLE_HEADER)
        {
            output = ReadData_GenericCSV(fileContentView, MainApplication::LoginDataFileType::Google);
        }
        else if (header == PASSWORD_DATA_FIREFOX_HEADER)
        {
            output = ReadData_GenericCSV(fileContentView, MainApplication::LoginDataFileType::Firefox);
        }
        else
        {
            throw hresult_invalid_argument(L"Invalid data type");
        }

        break;
    }

    default:
        throw hresult_invalid_argument(L"Invalid data type");
        break;
    }

    co_await PushData(output, import_type);
}

Windows::Foundation::IAsyncAction DataManager::ExportDataAsync(Windows::Storage::StorageFile const& file, Windows::Foundation::Collections::IVectorView<MainApplication::LoginData> const& data, const MainApplication::LoginDataFileType export_type) const
{
    LUPASS_LOG_FUNCTION();

    if (!file.IsAvailable())
    {
        throw hresult_invalid_argument(L"file is unavailable");
    }

    switch (export_type)
    {
    case MainApplication::LoginDataFileType::Lupass_Internal:

        co_await WriteData_Lupass(file, data);
        break;

    case MainApplication::LoginDataFileType::Lupass_External:
    case MainApplication::LoginDataFileType::Microsoft:
    case MainApplication::LoginDataFileType::Google:
    case MainApplication::LoginDataFileType::Firefox:
        co_await WriteData_GenericCSV(file, data, export_type);
        break;

    case MainApplication::LoginDataFileType::Kapersky:
        co_await WriteData_Kapersky(file, data);
        break;

    default:
        throw hresult_invalid_argument(L"Invalid data type");
        break;
    }
}

Windows::Foundation::IAsyncAction DataManager::PushData(const std::vector<MainApplication::LoginData>& data, const MainApplication::LoginDataFileType type)
{
    LUPASS_LOG_FUNCTION();

    if (data.empty())
    {
        return;
    }

    co_await InsertLoginDataAsync(data, true);
}

Windows::Foundation::IAsyncOperation<Windows::Foundation::Collections::IVector<MainApplication::LoginData>> DataManager::ReadData_Lupass(hstring const& display_name, Windows::Storage::Streams::IBuffer const& data_buffer)
{
    LUPASS_LOG_FUNCTION();

    if (data_buffer.Length() == 0)
    {
        co_return Windows::Foundation::Collections::IVector<MainApplication::LoginData>();
    }

    const hstring CRYPTOGRAPHY_KEY = display_name == APP_DATA_FILE_NAME ? KEY_MATERIAL_TEMP_ID : display_name;

    const auto provider = Windows::Security::Cryptography::Core::SymmetricKeyAlgorithmProvider::OpenAlgorithm(Windows::Security::Cryptography::Core::SymmetricAlgorithmNames::AesCbcPkcs7());
    const auto localSettings = Windows::Storage::ApplicationData::Current().LocalSettings();
    const auto keyMaterial_64Value = localSettings.Values().Lookup(CRYPTOGRAPHY_KEY).as<hstring>();
    const auto keyMaterial = Windows::Security::Cryptography::CryptographicBuffer::DecodeFromBase64String(keyMaterial_64Value);
    const auto key = provider.CreateSymmetricKey(keyMaterial);
    const auto decrypted_buffer = Windows::Security::Cryptography::Core::CryptographicEngine::Decrypt(key, data_buffer, nullptr);

    const hstring fileContent = Windows::Security::Cryptography::CryptographicBuffer::ConvertBinaryToString(Windows::Security::Cryptography::BinaryStringEncoding::Utf8, decrypted_buffer);

    std::vector<MainApplication::LoginData> output;

    const std::string std_content = to_string(fileContent);
    std::string line;
    for (auto iterator = std_content.begin(); iterator != std_content.end(); ++iterator)
    {
        if (*iterator == '\n')
        {
            ProcessCsvLine(to_hstring(line), output, MainApplication::LoginDataFileType::Lupass_Internal);
            line.clear();
        }
        else
        {
            line += *iterator;
        }
    }

    co_return winrt::single_threaded_vector<MainApplication::LoginData>(std::move(output));
}

Windows::Foundation::IAsyncAction DataManager::WriteData_Lupass(Windows::Storage::StorageFile const& file, Windows::Foundation::Collections::IVectorView<MainApplication::LoginData> const& data) const
{
    LUPASS_LOG_FUNCTION();

    hstring fileContent = to_hstring(PASSWORD_DATA_LUPASS_HEADER);
    for (const auto& iterator : data)
    {
        fileContent = fileContent + L"\n" + iterator.GetExportData(MainApplication::LoginDataFileType::Lupass_Internal);
    }

    fileContent = fileContent + L"\n";

    const auto data_buffer = Windows::Security::Cryptography::CryptographicBuffer::ConvertStringToBinary(fileContent, Windows::Security::Cryptography::BinaryStringEncoding::Utf8);
    const auto provider = Windows::Security::Cryptography::Core::SymmetricKeyAlgorithmProvider::OpenAlgorithm(Windows::Security::Cryptography::Core::SymmetricAlgorithmNames::AesCbcPkcs7());
    const auto keyMaterial = Windows::Security::Cryptography::CryptographicBuffer::GenerateRandom(provider.BlockLength());
    const auto key = provider.CreateSymmetricKey(keyMaterial);
    const auto encrypted_buffer = Windows::Security::Cryptography::Core::CryptographicEngine::Encrypt(key, data_buffer, nullptr);

    co_await Windows::Storage::FileIO::WriteBufferAsync(file, encrypted_buffer);

    const auto exportedKeyMaterial = Windows::Security::Cryptography::CryptographicBuffer::EncodeToBase64String(keyMaterial);
    const auto localSettings = Windows::Storage::ApplicationData::Current().LocalSettings();
    localSettings.Values().Insert(KEY_MATERIAL_TEMP_ID, box_value(exportedKeyMaterial));
}

std::vector<MainApplication::LoginData> DataManager::ReadData_GenericCSV(Windows::Foundation::Collections::IVectorView<hstring> const& file_text, const MainApplication::LoginDataFileType data_type)
{
    LUPASS_LOG_FUNCTION();

    std::vector<MainApplication::LoginData> output;

    for (const auto line : file_text)
    {
        ProcessCsvLine(line, output, data_type);
    }

    return output;
}

Windows::Foundation::IAsyncAction DataManager::WriteData_GenericCSV(Windows::Storage::StorageFile const& file, Windows::Foundation::Collections::IVectorView<MainApplication::LoginData> const& data, const MainApplication::LoginDataFileType data_type) const
{
    LUPASS_LOG_FUNCTION();

    Windows::Foundation::Collections::IVector<hstring> lines = single_threaded_vector<hstring>();

    switch (data_type)
    {
    case MainApplication::LoginDataFileType::Lupass_Internal:
    case MainApplication::LoginDataFileType::Lupass_External:
        lines.Append(to_hstring(PASSWORD_DATA_LUPASS_HEADER));
        break;

    case MainApplication::LoginDataFileType::Microsoft:
        lines.Append(to_hstring(PASSWORD_DATA_MICROSOFT_HEADER));
        break;

    case MainApplication::LoginDataFileType::Google:
        lines.Append(to_hstring(PASSWORD_DATA_GOOGLE_HEADER));
        break;

    case MainApplication::LoginDataFileType::Firefox:
        lines.Append(to_hstring(PASSWORD_DATA_FIREFOX_HEADER));
        break;

    default:
        throw hresult_invalid_argument(L"Invalid data type");
        break;
    }

    for (const MainApplication::LoginData& iterator : data)
    {
        lines.Append(iterator.GetExportData(data_type));
    }

    co_await Windows::Storage::FileIO::WriteLinesAsync(file, lines);
}

void DataManager::ProcessCsvLine(hstring const& line, std::vector<MainApplication::LoginData>& output_data, const MainApplication::LoginDataFileType data_type) const
{
    // LUPASS_LOG_FUNCTION();

    const std::string std_line = to_string(line);

    std::string header;
    switch (data_type)
    {
    case MainApplication::LoginDataFileType::Lupass_Internal:
    case MainApplication::LoginDataFileType::Lupass_External:
        header = to_string(PASSWORD_DATA_LUPASS_HEADER);
        break;

    case MainApplication::LoginDataFileType::Microsoft:
        header = to_string(PASSWORD_DATA_MICROSOFT_HEADER);
        break;

    case MainApplication::LoginDataFileType::Google:
        header = to_string(PASSWORD_DATA_GOOGLE_HEADER);
        break;

    case MainApplication::LoginDataFileType::Firefox:
        header = to_string(PASSWORD_DATA_FIREFOX_HEADER);
        break;

    default:
        throw hresult_invalid_argument(L"Invalid data type");
        break;
    }

    if (header.empty() || Helper::StringContains(std_line, header))
    {
        return;
    }

    const std::vector<hstring> headers = Helper::SplitString(header, ',', true);
    const std::vector<hstring> split_line = Helper::SplitString(std_line, ',', true);

    MainApplication::LoginData current_data = make<MainApplication::implementation::LoginData>();

    for (auto iterator = split_line.begin(); iterator != split_line.end(); ++iterator)
    {
        const unsigned current_index = static_cast<unsigned int>(std::distance(split_line.begin(), iterator));

        if (Helper::StringEquals(headers.at(current_index), L"name"))
        {
            current_data.Name(*iterator);
        }
        else if (Helper::StringEquals(headers.at(current_index), L"url"))
        {
            current_data.Url(*iterator);
        }
        else if (Helper::StringEquals(headers.at(current_index), L"username"))
        {
            current_data.Username(*iterator);
        }
        else if (Helper::StringEquals(headers.at(current_index), L"password"))
        {
            current_data.Password(*iterator);
        }
        else if (Helper::StringEquals(headers.at(current_index), L"notes"))
        {
            current_data.Notes(*iterator);
        }
        else if (Helper::StringContains(headers.at(current_index), L"created"))
        {
            current_data.Created(std::stoull(to_string(*iterator)));
        }
        else if (Helper::StringContains(headers.at(current_index), L"changed"))
        {
            current_data.Changed(std::stoull(to_string(*iterator)));
        }
        else if (Helper::StringContains(headers.at(current_index), L"used"))
        {
            current_data.Used(std::stoull(to_string(*iterator)));
        }

        if (current_index == split_line.size() - 1)
        {
            output_data.emplace_back(current_data);
        }
    }
}

std::vector<MainApplication::LoginData> DataManager::ReadData_Kapersky(const Windows::Foundation::Collections::IVectorView<hstring>& file_text)
{
    LUPASS_LOG_FUNCTION();

    MainApplication::LoginData new_data = make<MainApplication::implementation::LoginData>();
    std::vector<MainApplication::LoginData> output;

    for (const auto line : file_text)
    {
        if (Helper::StringEquals(line, L"Applications") || Helper::StringEquals(line, L"Notes"))
        {
            break;
        }

        const std::string std_line = to_string(line);

        if (!Helper::StringContains(std_line, ":"))
        {
            new_data.ResetLoginData();
            continue;
        }

        const unsigned int separatorIndex = static_cast<unsigned int>(std_line.find(':'));
        const std::string key = std_line.substr(0, separatorIndex);
        const std::string value = std_line.substr(static_cast<std::size_t>(separatorIndex + 2u), static_cast<std::size_t>(std_line.size() - separatorIndex - 1));

        if (Helper::StringEquals(key, "Website name"))
        {
            new_data.Name(to_hstring(value));
        }
        else if (Helper::StringEquals(key, "Website URL"))
        {
            new_data.Url(to_hstring(value));
        }
        else if (Helper::StringEquals(key, "Login"))
        {
            new_data.Username(to_hstring(value));
        }
        else if (Helper::StringEquals(key, "Password"))
        {
            new_data.Password(to_hstring(value));
        }
        else if (Helper::StringEquals(key, "Comment"))
        {
            new_data.Notes(to_hstring(value));

            output.emplace_back(new_data);
            new_data.ResetLoginData();
        }
    }

    return output;
}

Windows::Foundation::IAsyncAction DataManager::WriteData_Kapersky(Windows::Storage::StorageFile const& file, Windows::Foundation::Collections::IVectorView<MainApplication::LoginData> const& data) const
{
    LUPASS_LOG_FUNCTION();

    Windows::Foundation::Collections::IVector<hstring> lines = single_threaded_vector<hstring>();
    lines.Append(L"Websites\n");

    for (const MainApplication::LoginData& iterator : data)
    {
        lines.Append(iterator.GetExportData(MainApplication::LoginDataFileType::Kapersky));
    }

    co_await Windows::Storage::FileIO::WriteLinesAsync(file, lines);
}