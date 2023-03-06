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
	Windows::Foundation::IAsyncAction LoginDataManager::ImportDataAsync(const Windows::Storage::StorageFile& file)
	{
		LUPASS_LOG_FUNCTION();

		if (!file.IsAvailable())
		{
			throw hresult_invalid_argument(L"file is unavailable");
		}

		const hstring fileType = file.FileType();
		
		if (fileType == L".bin")
		{
			ReadBinaryData(file);
			co_return;
		}

		const auto fileContent = (co_await Windows::Storage::FileIO::ReadLinesAsync(file)).GetView();

		if (fileType == L".txt")
		{
			ReadTextData(fileContent);
		}
		else if (fileType == L".csv")
		{
			ReadCsvData(fileContent);
		}
		else
		{
			throw hresult_not_implemented(L"not implemented yet");
		}

		co_return;
	}

	Windows::Foundation::IAsyncAction LoginDataManager::ExportDataAsync(const Windows::Storage::StorageFile& file, const Windows::Foundation::Collections::IVectorView<PasswordManager::LoginData>& data)
	{
		LUPASS_LOG_FUNCTION();

		if (!file.IsAvailable())
		{
			throw hresult_invalid_argument(L"file is unavailable");
		}

		const hstring fileType = file.FileType();
		
		if (fileType == L".bin")
		{
			WriteBinaryDataAsync(file, data);
		}
		else if (fileType == L".txt")
		{
			WriteTextDataAsync(file, data);
		}
		else if (fileType == L".csv")
		{
			WriteCsvDataAsync(file, data);
		}
		else
		{
			throw hresult_not_implemented(L"not implemented yet");
		}

		co_return;
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

	void LoginDataManager::PushData(const PasswordManager::LoginData& data, const PasswordManager::LoginDataFileType& type)
	{
		LUPASS_LOG_FUNCTION();

		m_data_updated(*this, make<PasswordManager::implementation::LoginUpdateEventParams>(data, type));
	}

	Windows::Foundation::IAsyncAction LoginDataManager::ReadBinaryData(const Windows::Storage::StorageFile& file)
	{
		LUPASS_LOG_FUNCTION();

		const auto data_buffer = co_await Windows::Storage::FileIO::ReadBufferAsync(file);
		const auto provider = Windows::Security::Cryptography::Core::SymmetricKeyAlgorithmProvider::OpenAlgorithm(Windows::Security::Cryptography::Core::SymmetricAlgorithmNames::AesCbcPkcs7());
		const auto localSettings = Windows::Storage::ApplicationData::Current().LocalSettings();
		const auto keyMaterial_64Value = localSettings.Values().Lookup(KEY_MATERIAL_TEMP_ID).as<hstring>();
		const auto keyMaterial = Windows::Security::Cryptography::CryptographicBuffer::DecodeFromBase64String(keyMaterial_64Value);
		const auto key = provider.CreateSymmetricKey(keyMaterial);
		const auto decrypted_buffer = Windows::Security::Cryptography::Core::CryptographicEngine::Decrypt(key, data_buffer, nullptr);

		const hstring fileContent = Windows::Security::Cryptography::CryptographicBuffer::ConvertBinaryToString(Windows::Security::Cryptography::BinaryStringEncoding::Utf8, decrypted_buffer);

		PasswordManager::LoginData newData = make<PasswordManager::implementation::LoginData>();

		const std::string std_content = to_string(fileContent);
		std::string line;
		for (auto iterator = std_content.begin(); iterator != std_content.end(); ++iterator)
		{
			if (*iterator == '\n' || iterator == std_content.end() - 1)
			{
				ProcessCsvLine(to_hstring(line), newData, PasswordManager::LoginDataFileType::BIN);
				line.clear();
			}
			else
			{
				line += *iterator;
			}
		}
	}

	Windows::Foundation::IAsyncAction LoginDataManager::WriteBinaryDataAsync(const Windows::Storage::StorageFile& file, const Windows::Foundation::Collections::IVectorView<PasswordManager::LoginData>& data)
	{
		LUPASS_LOG_FUNCTION();

		hstring fileContent = to_hstring(PASSWORD_DATA_CSV_HEADER);
		for (const auto& iterator : data)
		{
			fileContent = fileContent + L"\n" + iterator.GetDataAsString(PasswordManager::LoginDataFileType::BIN);
		}
		
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

	void LoginDataManager::ReadTextData(const Windows::Foundation::Collections::IVectorView<hstring>& file_text)
	{
		LUPASS_LOG_FUNCTION();

		PasswordManager::LoginData newData = make<PasswordManager::implementation::LoginData>();

		for (const auto line : file_text)
		{
			const std::string std_line = to_string(line);

			if (!Helper::StringContains(std_line, ":"))
			{
				newData.resetLoginData();
				continue;
			}

			if (Helper::StringContains(std_line, "Application") || Helper::StringContains(std_line, "Notes"))
			{
				break;
			}

			const unsigned int separatorIndex = static_cast<unsigned int>(std_line.find(':'));
			const std::string key = std_line.substr(0, separatorIndex);
			const std::string value = std_line.substr(separatorIndex + 2u, std_line.size() - separatorIndex - 1);

			if (key == "Website name")
			{
				newData.Name(to_hstring(value));
			}
			else if (key == "Website URL")
			{
				newData.Url(to_hstring(value));
			}
			else if (key == "Login")
			{
				newData.Username(to_hstring(value));
			}
			else if (key == "Password")
			{
				newData.Password(to_hstring(value));

				PushData(newData, PasswordManager::LoginDataFileType::TXT);
				newData.resetLoginData();
			}
		}
	}

	void LoginDataManager::WriteTextDataAsync(const Windows::Storage::StorageFile& file, const Windows::Foundation::Collections::IVectorView<PasswordManager::LoginData>& data) const
	{
		LUPASS_LOG_FUNCTION();

		Windows::Foundation::Collections::IVector<hstring> lines = single_threaded_vector<hstring>();
		lines.Append(L"Websites\n");

		for (const PasswordManager::LoginData& iterator : data)
		{
			lines.Append(iterator.GetDataAsString(PasswordManager::LoginDataFileType::TXT));
		}

		Windows::Storage::FileIO::WriteLinesAsync(file, lines);
	}

	void LoginDataManager::ReadCsvData(const Windows::Foundation::Collections::IVectorView<hstring>& file_text)
	{
		LUPASS_LOG_FUNCTION();

		PasswordManager::LoginData newData = make<PasswordManager::implementation::LoginData>();

		for (const auto line : file_text)
		{
			ProcessCsvLine(line, newData, PasswordManager::LoginDataFileType::CSV);
		}
	}

	void LoginDataManager::WriteCsvDataAsync(const Windows::Storage::StorageFile& file, const Windows::Foundation::Collections::IVectorView<PasswordManager::LoginData>& data) const
	{
		LUPASS_LOG_FUNCTION();

		Windows::Foundation::Collections::IVector<hstring> lines = single_threaded_vector<hstring>();
		lines.Append(to_hstring(PASSWORD_DATA_CSV_HEADER));

		for (const PasswordManager::LoginData& iterator : data)
		{
			lines.Append(iterator.GetDataAsString(PasswordManager::LoginDataFileType::CSV));
		}

		Windows::Storage::FileIO::WriteLinesAsync(file, lines);
	}
	
	void LoginDataManager::ProcessCsvLine(const hstring line, PasswordManager::LoginData& current_data, const PasswordManager::LoginDataFileType& data_type)
	{
		LUPASS_LOG_FUNCTION();

		const std::string std_line = to_string(line);

		if (Helper::StringContains(std_line, to_string(PASSWORD_DATA_CSV_HEADER).substr(0, to_string(PASSWORD_DATA_CSV_HEADER).size() - 1)))
		{
			return;
		}

		constexpr unsigned int num_columns = 4u;
		unsigned int currentSeparatorIndex = 0u;

		for (unsigned int iterator = 0u; iterator < num_columns; ++iterator)
		{
			const unsigned int nextSeparatorIndex = static_cast<unsigned int>(iterator == num_columns - 1 ? std_line.size() : std_line.find(',', currentSeparatorIndex));

			const hstring converted_value = to_hstring(std_line.substr(currentSeparatorIndex, nextSeparatorIndex - currentSeparatorIndex));
			switch (iterator)
			{
				case 0u:
					current_data.Name(converted_value);
					break;

				case 1u:
					current_data.Url(converted_value);
					break;

				case 2u:
					current_data.Username(converted_value);
					break;

				case 3u:
					current_data.Password(converted_value);
					PushData(current_data, data_type);
					current_data.resetLoginData();
					currentSeparatorIndex = 0u;
					continue;
			}

			currentSeparatorIndex = nextSeparatorIndex + 1u;
		}
	}
}
