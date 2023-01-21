// Author: Lucas Oliveira Vilas-Bôas
// Year: 2022
// Repository: https://github.com/lucoiso/PasswordManagement

#include "pch.h"

#include "LoginDataManager.h"
#include "LoginDataManager.g.cpp"

#include "Helper.h"
#include "LoginUpdateEventParams.h"

#include <filesystem>
#include <fstream>
#include <string>
#include <LoginData.h>

using namespace winrt;

namespace winrt::PasswordManager::implementation
{
	Windows::Foundation::IAsyncAction LoginDataManager::ImportDataAsync(const Windows::Storage::StorageFile& file)
	{
		if (!file.IsAvailable())
		{
			throw hresult_invalid_argument(L"file is unavailable");
		}
		const hstring fileType = file.FileType();

		const auto fileContent = (co_await Windows::Storage::FileIO::ReadLinesAsync(file)).GetView();
		
		if (fileType == L".bin")
		{
			ReadBinaryData(fileContent);
		}
		else if (fileType == L".txt")
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
		return m_data_updated.add(handler);
	}

	void LoginDataManager::LoginDataUpdated(event_token const& token) noexcept
	{
		m_data_updated.remove(token);
	}

	void LoginDataManager::PushData(const PasswordManager::LoginData& data, const PasswordManager::LoginDataFileType& type)
	{
		m_data_updated(*this, make<PasswordManager::implementation::LoginUpdateEventParams>(data, type));
	}

	void LoginDataManager::ReadBinaryData(const Windows::Foundation::Collections::IVectorView<hstring>& file_text)
	{
		hstring fileContent;
		for (const auto& iterator : file_text)
		{
			fileContent = fileContent + iterator;
		}
		
		const auto decoded_buffer = Windows::Security::Cryptography::CryptographicBuffer::DecodeFromBase64String(fileContent);
		fileContent = Windows::Security::Cryptography::CryptographicBuffer::ConvertBinaryToString(Windows::Security::Cryptography::BinaryStringEncoding::Utf8, decoded_buffer);
		
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

	Windows::Foundation::IAsyncAction LoginDataManager::WriteBinaryDataAsync(const Windows::Storage::StorageFile& file, const Windows::Foundation::Collections::IVectorView<PasswordManager::LoginData>& data) const
	{
		hstring fileContent = L"name,url,username,password";
		for (const auto& iterator : data)
		{
			fileContent = fileContent + L"\n" + iterator.GetDataAsString(PasswordManager::LoginDataFileType::BIN);
		}
		
		const auto data_buffer = Windows::Security::Cryptography::CryptographicBuffer::ConvertStringToBinary(fileContent, Windows::Security::Cryptography::BinaryStringEncoding::Utf8);
		const hstring encodedContent = Windows::Security::Cryptography::CryptographicBuffer::EncodeToBase64String(data_buffer);

		co_await Windows::Storage::FileIO::WriteTextAsync(file, encodedContent);
	}

	void LoginDataManager::ReadTextData(const Windows::Foundation::Collections::IVectorView<hstring>& file_text)
	{
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
			const std::string value = std_line.substr(separatorIndex + 2, std_line.size() - separatorIndex - 1);

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

	Windows::Foundation::IAsyncAction LoginDataManager::WriteTextDataAsync([[maybe_unused]] const Windows::Storage::StorageFile& file, [[maybe_unused]] const Windows::Foundation::Collections::IVectorView<PasswordManager::LoginData>& data) const
	{
		throw hresult_not_implemented(L"not implemented yet");
	}

	void LoginDataManager::ReadCsvData(const Windows::Foundation::Collections::IVectorView<hstring>& file_text)
	{
		PasswordManager::LoginData newData = make<PasswordManager::implementation::LoginData>();

		for (const auto line : file_text)
		{
			ProcessCsvLine(line, newData, PasswordManager::LoginDataFileType::CSV);
		}
	}

	Windows::Foundation::IAsyncAction LoginDataManager::WriteCsvDataAsync(const Windows::Storage::StorageFile& file, const Windows::Foundation::Collections::IVectorView<PasswordManager::LoginData>& data) const
	{
		Windows::Foundation::Collections::IVector<hstring> lines = single_threaded_vector<hstring>();
		lines.Append(L"name,url,username,password");

		for (const PasswordManager::LoginData& iterator : data)
		{
			lines.Append(iterator.GetDataAsString(PasswordManager::LoginDataFileType::CSV));
		}

		co_await Windows::Storage::FileIO::WriteLinesAsync(file, lines);
	}
	
	void LoginDataManager::ProcessCsvLine(const hstring line, PasswordManager::LoginData& current_data, const PasswordManager::LoginDataFileType& data_type)
	{
		const std::string std_line = to_string(line);

		if (Helper::StringContains(std_line, "name,url,username,password"))
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
