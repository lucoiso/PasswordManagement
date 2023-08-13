// Author: Lucas Oliveira Vilas-Bôas
// Year: 2023
// Repository: https://github.com/lucoiso/PasswordManagement

#include "pch.h"

#include "LoginDataManager.h"
#include "LoginDataManager.g.cpp"

#include "LoginData.h"
#include "LoginUpdateEventParams.h"

#include <filesystem>
#include <fstream>
#include <string>

using namespace winrt;

namespace winrt::PasswordManager::implementation
{
    Windows::Foundation::IAsyncAction LoginDataManager::ImportDataAsync(Windows::Storage::StorageFile const file, const PasswordManager::LoginDataFileType import_type)
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

        std::vector<PasswordManager::LoginData> output;

        if (import_type == PasswordManager::LoginDataFileType::Lupass_Internal)
        {
            try
            {
                const hstring file_name = file.DisplayName();
                const auto data_buffer = co_await Windows::Storage::FileIO::ReadBufferAsync(file);
                for (const auto& import_result_it : co_await ReadData_Lupass(file_name, data_buffer))
                {
                    output.emplace_back(import_result_it);
                }
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
        case PasswordManager::LoginDataFileType::Lupass_External:
        case PasswordManager::LoginDataFileType::Microsoft:
        case PasswordManager::LoginDataFileType::Google:
        case PasswordManager::LoginDataFileType::Firefox:
            output = ReadData_GenericCSV(fileContentView, import_type);
            break;

        case PasswordManager::LoginDataFileType::Kapersky:
            output = ReadData_Kapersky(fileContentView);
            break;

        case PasswordManager::LoginDataFileType::GenericCSV:
        {
            const hstring header = fileContentView.GetAt(0);

            if (header == PASSWORD_DATA_LUPASS_HEADER)
            {
                output = ReadData_GenericCSV(fileContentView, PasswordManager::LoginDataFileType::Lupass_External);
            }
            else if (header == PASSWORD_DATA_MICROSOFT_HEADER)
            {
                output = ReadData_GenericCSV(fileContentView, PasswordManager::LoginDataFileType::Microsoft);
            }
            else if (header == PASSWORD_DATA_GOOGLE_HEADER)
            {
                output = ReadData_GenericCSV(fileContentView, PasswordManager::LoginDataFileType::Google);
            }
            else if (header == PASSWORD_DATA_FIREFOX_HEADER)
            {
                output = ReadData_GenericCSV(fileContentView, PasswordManager::LoginDataFileType::Firefox);
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

        PushData(output, import_type);
    }

    Windows::Foundation::IAsyncAction LoginDataManager::ExportDataAsync(Windows::Storage::StorageFile const file, Windows::Foundation::Collections::IVectorView<PasswordManager::LoginData> const& data, const PasswordManager::LoginDataFileType export_type)
    {
        LUPASS_LOG_FUNCTION();

        if (!file.IsAvailable())
        {
            throw hresult_invalid_argument(L"file is unavailable");
        }

        switch (export_type)
        {
        case PasswordManager::LoginDataFileType::Lupass_Internal:
            co_await WriteData_Lupass(file, data);
            break;

        case PasswordManager::LoginDataFileType::Lupass_External:
        case PasswordManager::LoginDataFileType::Microsoft:
        case PasswordManager::LoginDataFileType::Google:
        case PasswordManager::LoginDataFileType::Firefox:
            co_await WriteData_GenericCSV(file, data, export_type);
            break;

        case PasswordManager::LoginDataFileType::Kapersky:
            co_await WriteData_Kapersky(file, data);
            break;

        default:
            throw hresult_invalid_argument(L"Invalid data type");
            break;
        }
    }

    event_token LoginDataManager::LoginDataUpdated(Windows::Foundation::EventHandler<PasswordManager::LoginUpdateEventParams> const& handler)
    {
        LUPASS_LOG_FUNCTION();

        return m_data_updated.add(handler);
    }

    void LoginDataManager::LoginDataUpdated(event_token const& token) noexcept
    {
        LUPASS_LOG_FUNCTION();

        m_data_updated.remove(token);
    }

    void LoginDataManager::PushData(std::vector<PasswordManager::LoginData>& data, const PasswordManager::LoginDataFileType type)
    {
        LUPASS_LOG_FUNCTION();

        if (data.empty())
        {
            return;
        }

        const auto data_to_send = winrt::single_threaded_vector(std::move(data));
        m_data_updated(*this, make<PasswordManager::implementation::LoginUpdateEventParams>(data_to_send, type));
    }

    Windows::Foundation::IAsyncOperation<Windows::Foundation::Collections::IVector<PasswordManager::LoginData>> LoginDataManager::ReadData_Lupass(hstring const& display_name, Windows::Storage::Streams::IBuffer const& data_buffer)
    {
        LUPASS_LOG_FUNCTION();

        if (data_buffer.Length() == 0)
        {
            co_return Windows::Foundation::Collections::IVector<PasswordManager::LoginData>();
        }

        const hstring CRYPTOGRAPHY_KEY = display_name == APP_DATA_FILE_NAME ? KEY_MATERIAL_TEMP_ID : display_name;

        const auto provider = Windows::Security::Cryptography::Core::SymmetricKeyAlgorithmProvider::OpenAlgorithm(Windows::Security::Cryptography::Core::SymmetricAlgorithmNames::AesCbcPkcs7());
        const auto localSettings = Windows::Storage::ApplicationData::Current().LocalSettings();
        const auto keyMaterial_64Value = localSettings.Values().Lookup(CRYPTOGRAPHY_KEY).as<hstring>();
        const auto keyMaterial = Windows::Security::Cryptography::CryptographicBuffer::DecodeFromBase64String(keyMaterial_64Value);
        const auto key = provider.CreateSymmetricKey(keyMaterial);
        const auto decrypted_buffer = Windows::Security::Cryptography::Core::CryptographicEngine::Decrypt(key, data_buffer, nullptr);

        const hstring fileContent = Windows::Security::Cryptography::CryptographicBuffer::ConvertBinaryToString(Windows::Security::Cryptography::BinaryStringEncoding::Utf8, decrypted_buffer);

        std::vector<PasswordManager::LoginData> output;

        const std::string std_content = to_string(fileContent);
        std::string line;
        for (auto iterator = std_content.begin(); iterator != std_content.end(); ++iterator)
        {
            if (*iterator == '\n')
            {
                ProcessCsvLine(to_hstring(line), output, PasswordManager::LoginDataFileType::Lupass_Internal);
                line.clear();
            }
            else
            {
                line += *iterator;
            }
        }

        co_return winrt::single_threaded_vector<PasswordManager::LoginData>(std::move(output));
    }

    Windows::Foundation::IAsyncAction LoginDataManager::WriteData_Lupass(Windows::Storage::StorageFile const& file, Windows::Foundation::Collections::IVectorView<PasswordManager::LoginData> const& data)
    {
        LUPASS_LOG_FUNCTION();

        hstring fileContent = to_hstring(PASSWORD_DATA_LUPASS_HEADER);
        for (const auto& iterator : data)
        {
            fileContent = fileContent + L"\n" + iterator.GetExportData(PasswordManager::LoginDataFileType::Lupass_Internal);
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

    std::vector<PasswordManager::LoginData> LoginDataManager::ReadData_GenericCSV(Windows::Foundation::Collections::IVectorView<hstring> const& file_text, const PasswordManager::LoginDataFileType data_type)
    {
        LUPASS_LOG_FUNCTION();

        std::vector<PasswordManager::LoginData> output;

        for (const auto line : file_text)
        {
            ProcessCsvLine(line, output, data_type);
        }

        return output;
    }

    Windows::Foundation::IAsyncAction LoginDataManager::WriteData_GenericCSV(Windows::Storage::StorageFile const& file, Windows::Foundation::Collections::IVectorView<PasswordManager::LoginData> const& data, const PasswordManager::LoginDataFileType data_type) const
    {
        LUPASS_LOG_FUNCTION();

        Windows::Foundation::Collections::IVector<hstring> lines = single_threaded_vector<hstring>();

        switch (data_type)
        {
        case PasswordManager::LoginDataFileType::Lupass_Internal:
        case PasswordManager::LoginDataFileType::Lupass_External:
            lines.Append(to_hstring(PASSWORD_DATA_LUPASS_HEADER));
            break;

        case PasswordManager::LoginDataFileType::Microsoft:
            lines.Append(to_hstring(PASSWORD_DATA_MICROSOFT_HEADER));
            break;

        case PasswordManager::LoginDataFileType::Google:
            lines.Append(to_hstring(PASSWORD_DATA_GOOGLE_HEADER));
            break;

        case PasswordManager::LoginDataFileType::Firefox:
            lines.Append(to_hstring(PASSWORD_DATA_FIREFOX_HEADER));
            break;

        default:
            throw hresult_invalid_argument(L"Invalid data type");
            break;
        }

        for (const PasswordManager::LoginData& iterator : data)
        {
            lines.Append(iterator.GetExportData(data_type));
        }

        co_await Windows::Storage::FileIO::WriteLinesAsync(file, lines);
    }

    void LoginDataManager::ProcessCsvLine(hstring const& line, std::vector<PasswordManager::LoginData>& output_data, const PasswordManager::LoginDataFileType data_type)
    {
        // LUPASS_LOG_FUNCTION();

        const std::string std_line = to_string(line);

        std::string header;
        switch (data_type)
        {
        case PasswordManager::LoginDataFileType::Lupass_Internal:
        case PasswordManager::LoginDataFileType::Lupass_External:
            header = to_string(PASSWORD_DATA_LUPASS_HEADER);
            break;

        case PasswordManager::LoginDataFileType::Microsoft:
            header = to_string(PASSWORD_DATA_MICROSOFT_HEADER);
            break;

        case PasswordManager::LoginDataFileType::Google:
            header = to_string(PASSWORD_DATA_GOOGLE_HEADER);
            break;

        case PasswordManager::LoginDataFileType::Firefox:
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

        PasswordManager::LoginData current_data = make<PasswordManager::implementation::LoginData>();

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

    std::vector<PasswordManager::LoginData> LoginDataManager::ReadData_Kapersky(const Windows::Foundation::Collections::IVectorView<hstring>& file_text)
    {
        LUPASS_LOG_FUNCTION();

        PasswordManager::LoginData new_data = make<PasswordManager::implementation::LoginData>();
        std::vector<PasswordManager::LoginData> output;

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

    Windows::Foundation::IAsyncAction LoginDataManager::WriteData_Kapersky(Windows::Storage::StorageFile const& file, Windows::Foundation::Collections::IVectorView<PasswordManager::LoginData> const& data) const
    {
        LUPASS_LOG_FUNCTION();

        Windows::Foundation::Collections::IVector<hstring> lines = single_threaded_vector<hstring>();
        lines.Append(L"Websites\n");

        for (const PasswordManager::LoginData& iterator : data)
        {
            lines.Append(iterator.GetExportData(PasswordManager::LoginDataFileType::Kapersky));
        }

        co_await Windows::Storage::FileIO::WriteLinesAsync(file, lines);
    }
}