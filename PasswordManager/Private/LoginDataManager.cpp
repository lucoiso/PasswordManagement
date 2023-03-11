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
	Windows::Foundation::IAsyncAction LoginDataManager::ImportDataAsync(const Windows::Storage::StorageFile& file, const PasswordManager::LoginDataExportType export_type)
	{
		LUPASS_LOG_FUNCTION();

		if (!file.IsAvailable())
		{
			throw hresult_invalid_argument(L"file is unavailable");
		}
		
		if (export_type == PasswordManager::LoginDataExportType::Lupass_Internal)
		{
			co_await ReadData_Lupass(file);
			co_return;
		}

		const auto fileContent = (co_await Windows::Storage::FileIO::ReadLinesAsync(file)).GetView();

		if (fileContent.Size() == 0)
		{
			co_return;
		}

		switch (export_type)
		{
			case PasswordManager::LoginDataExportType::Lupass_External:
			case PasswordManager::LoginDataExportType::Microsoft:
			case PasswordManager::LoginDataExportType::Google:
			case PasswordManager::LoginDataExportType::Firefox:
				ReadData_GenericCSV(fileContent, export_type);
				break;

			case PasswordManager::LoginDataExportType::Kapersky:
				ReadData_Kapersky(fileContent);
				break;

			case PasswordManager::LoginDataExportType::GenericCSV:
			{
				const hstring header = fileContent.GetAt(0);

				if (header == PASSWORD_DATA_LUPASS_HEADER)
				{
					ReadData_GenericCSV(fileContent, PasswordManager::LoginDataExportType::Lupass_External);
				}
				else if (header == PASSWORD_DATA_MICROSOFT_HEADER)
				{
					ReadData_GenericCSV(fileContent, PasswordManager::LoginDataExportType::Microsoft);
				}
				else if (header == PASSWORD_DATA_GOOGLE_HEADER)
				{
					ReadData_GenericCSV(fileContent, PasswordManager::LoginDataExportType::Google);
				}
				else if (header == PASSWORD_DATA_FIREFOX_HEADER)
				{
					ReadData_GenericCSV(fileContent, PasswordManager::LoginDataExportType::Firefox);
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
	}

	Windows::Foundation::IAsyncAction LoginDataManager::ExportDataAsync(const Windows::Storage::StorageFile& file, const Windows::Foundation::Collections::IVectorView<PasswordManager::LoginData>& data, const PasswordManager::LoginDataExportType export_type)
	{
		LUPASS_LOG_FUNCTION();

		if (!file.IsAvailable())
		{
			throw hresult_invalid_argument(L"file is unavailable");
		}

		switch (export_type)
		{
			case PasswordManager::LoginDataExportType::Lupass_Internal:
				co_await WriteData_Lupass(file, data);
				break;

			case PasswordManager::LoginDataExportType::Lupass_External:
			case PasswordManager::LoginDataExportType::Microsoft:
			case PasswordManager::LoginDataExportType::Google:
			case PasswordManager::LoginDataExportType::Firefox:
				co_await WriteData_GenericCSV(file, data, export_type);
				break;

			case PasswordManager::LoginDataExportType::Kapersky:
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

	void LoginDataManager::PushData(const PasswordManager::LoginData& data, const PasswordManager::LoginDataExportType& type)
	{
		LUPASS_LOG_FUNCTION();

		m_data_updated(*this, make<PasswordManager::implementation::LoginUpdateEventParams>(data, type));
	}

	Windows::Foundation::IAsyncAction LoginDataManager::ReadData_Lupass(const Windows::Storage::StorageFile& file)
	{
		LUPASS_LOG_FUNCTION();

		const hstring file_name = file.DisplayName();
		const auto data_buffer = co_await Windows::Storage::FileIO::ReadBufferAsync(file);

		if (data_buffer.Length() == 0)
		{
			co_return;
		}

		const hstring CRYPTOGRAPHY_KEY = file_name == APP_DATA_FILE_NAME ? KEY_MATERIAL_TEMP_ID : file_name;

		const auto provider = Windows::Security::Cryptography::Core::SymmetricKeyAlgorithmProvider::OpenAlgorithm(Windows::Security::Cryptography::Core::SymmetricAlgorithmNames::AesCbcPkcs7());
		const auto localSettings = Windows::Storage::ApplicationData::Current().LocalSettings();
		const auto keyMaterial_64Value = localSettings.Values().Lookup(CRYPTOGRAPHY_KEY).as<hstring>();
		const auto keyMaterial = Windows::Security::Cryptography::CryptographicBuffer::DecodeFromBase64String(keyMaterial_64Value);
		const auto key = provider.CreateSymmetricKey(keyMaterial);
		const auto decrypted_buffer = Windows::Security::Cryptography::Core::CryptographicEngine::Decrypt(key, data_buffer, nullptr);

		const hstring fileContent = Windows::Security::Cryptography::CryptographicBuffer::ConvertBinaryToString(Windows::Security::Cryptography::BinaryStringEncoding::Utf8, decrypted_buffer);

		PasswordManager::LoginData newData = make<PasswordManager::implementation::LoginData>();

		const std::string std_content = to_string(fileContent);
		std::string line;
		for (auto iterator = std_content.begin(); iterator != std_content.end(); ++iterator)
		{
			if (*iterator == '\n')
			{
				ProcessCsvLine(to_hstring(line), newData, PasswordManager::LoginDataExportType::Lupass_Internal);
				line.clear();
			}
			else
			{
				line += *iterator;
			}
		}
	}

	Windows::Foundation::IAsyncAction LoginDataManager::WriteData_Lupass(const Windows::Storage::StorageFile& file, const Windows::Foundation::Collections::IVectorView<PasswordManager::LoginData>& data)
	{
		LUPASS_LOG_FUNCTION();

		hstring fileContent = to_hstring(PASSWORD_DATA_LUPASS_HEADER);
		for (const auto& iterator : data)
		{
			fileContent = fileContent + L"\n" + iterator.GetExportData(PasswordManager::LoginDataExportType::Lupass_Internal);
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

	void LoginDataManager::ReadData_GenericCSV(const Windows::Foundation::Collections::IVectorView<hstring>& file_text, const PasswordManager::LoginDataExportType& data_type)
	{
		LUPASS_LOG_FUNCTION();

		PasswordManager::LoginData newData = make<PasswordManager::implementation::LoginData>();

		for (const auto line : file_text)
		{
			ProcessCsvLine(line, newData, data_type);
		}
	}

	Windows::Foundation::IAsyncAction LoginDataManager::WriteData_GenericCSV(const Windows::Storage::StorageFile& file, const Windows::Foundation::Collections::IVectorView<PasswordManager::LoginData>& data, const PasswordManager::LoginDataExportType& data_type) const
	{
		LUPASS_LOG_FUNCTION();

		Windows::Foundation::Collections::IVector<hstring> lines = single_threaded_vector<hstring>();

		switch (data_type)
		{
			case PasswordManager::LoginDataExportType::Lupass_Internal:
			case PasswordManager::LoginDataExportType::Lupass_External:
				lines.Append(to_hstring(PASSWORD_DATA_LUPASS_HEADER));
				break;

			case PasswordManager::LoginDataExportType::Microsoft:
				lines.Append(to_hstring(PASSWORD_DATA_MICROSOFT_HEADER));
				break;

			case PasswordManager::LoginDataExportType::Google:
				lines.Append(to_hstring(PASSWORD_DATA_GOOGLE_HEADER));
				break;

			case PasswordManager::LoginDataExportType::Firefox:
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

	void LoginDataManager::ProcessCsvLine(const hstring line, PasswordManager::LoginData& current_data, const PasswordManager::LoginDataExportType& data_type)
	{
		LUPASS_LOG_FUNCTION();

		const std::string std_line = to_string(line);

		std::string header;
		switch (data_type)
		{
			case PasswordManager::LoginDataExportType::Lupass_Internal:
			case PasswordManager::LoginDataExportType::Lupass_External:
				header = to_string(PASSWORD_DATA_LUPASS_HEADER);
				break;

			case PasswordManager::LoginDataExportType::Microsoft:
				header = to_string(PASSWORD_DATA_MICROSOFT_HEADER);
				break;

			case PasswordManager::LoginDataExportType::Google:
				header = to_string(PASSWORD_DATA_GOOGLE_HEADER);
				break;

			case PasswordManager::LoginDataExportType::Firefox:
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
				PushData(current_data, data_type);
				current_data.ResetLoginData();
			}
		}
	}

	void LoginDataManager::ReadData_Kapersky(const Windows::Foundation::Collections::IVectorView<hstring>& file_text)
	{
		LUPASS_LOG_FUNCTION();

		PasswordManager::LoginData new_data = make<PasswordManager::implementation::LoginData>();

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

				PushData(new_data, PasswordManager::LoginDataExportType::Kapersky);
				new_data.ResetLoginData();
			}
		}
	}

	Windows::Foundation::IAsyncAction LoginDataManager::WriteData_Kapersky(const Windows::Storage::StorageFile& file, const Windows::Foundation::Collections::IVectorView<PasswordManager::LoginData>& data) const
	{
		LUPASS_LOG_FUNCTION();

		Windows::Foundation::Collections::IVector<hstring> lines = single_threaded_vector<hstring>();
		lines.Append(L"Websites\n");

		for (const PasswordManager::LoginData& iterator : data)
		{
			lines.Append(iterator.GetExportData(PasswordManager::LoginDataExportType::Kapersky));
		}

		co_await Windows::Storage::FileIO::WriteLinesAsync(file, lines);
	}
}
