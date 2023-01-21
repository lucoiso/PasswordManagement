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
	Windows::Foundation::IAsyncAction LoginDataManager::importData(const Windows::Storage::StorageFile& file)
	{
		if (!file.IsAvailable())
		{
			throw hresult_invalid_argument(L"file is unavailable");
		}
		const hstring fileType = file.FileType();

		const auto fileContent = (co_await Windows::Storage::FileIO::ReadLinesAsync(file)).GetView();
		
		if (fileType == L".bin")
		{
			readBinaryData(fileContent);
		}
		else if (fileType == L".txt")
		{
			readTextData(fileContent);
		}
		else if (fileType == L".csv")
		{
			readCsvData(fileContent);
		}
		else
		{
			throw hresult_not_implemented(L"not implemented yet");
		}

		co_return;
	}

	Windows::Foundation::IAsyncAction LoginDataManager::exportData(const Windows::Storage::StorageFile& file, const Windows::Foundation::Collections::IVectorView<PasswordManager::LoginData>& data)
	{
		if (!file.IsAvailable())
		{
			throw hresult_invalid_argument(L"file is unavailable");
		}

		const hstring fileType = file.FileType();
		
		if (fileType == L".bin")
		{
			writeBinaryData(file, data);
		}
		else if (fileType == L".txt")
		{
			writeTextData(file, data);
		}
		else if (fileType == L".csv")
		{
			writeCsvData(file, data);
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

	void LoginDataManager::sendData(const PasswordManager::LoginData& data, const PasswordManager::LoginDataFileType& type)
	{
		m_data_updated(*this, make<PasswordManager::implementation::LoginUpdateEventParams>(data, type));
	}

	void LoginDataManager::readBinaryData(const Windows::Foundation::Collections::IVectorView<hstring>& file_text)
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
		for (const auto& iterator : std_content)
		{
			if (iterator == '\n')
			{
				processCsvLine(to_hstring(line), newData, PasswordManager::LoginDataFileType::BIN);
				line.clear();
			}
			else
			{
				line += iterator;
			}
		}
	}

	Windows::Foundation::IAsyncAction LoginDataManager::writeBinaryData(const Windows::Storage::StorageFile& file, const Windows::Foundation::Collections::IVectorView<PasswordManager::LoginData>& data) const
	{
		hstring fileContent = L"name,url,username,password";
		for (const auto& iterator : data)
		{
			fileContent = fileContent + L"\n" + iterator.getDataAsString(PasswordManager::LoginDataFileType::BIN);
		}
		
		const auto data_buffer = Windows::Security::Cryptography::CryptographicBuffer::ConvertStringToBinary(fileContent, Windows::Security::Cryptography::BinaryStringEncoding::Utf8);
		const hstring encodedContent = Windows::Security::Cryptography::CryptographicBuffer::EncodeToBase64String(data_buffer);

		co_await Windows::Storage::FileIO::WriteTextAsync(file, encodedContent);
	}

	void LoginDataManager::readTextData(const Windows::Foundation::Collections::IVectorView<hstring>& file_text)
	{
		PasswordManager::LoginData newData = make<PasswordManager::implementation::LoginData>();

		for (const auto line : file_text)
		{
			const std::string std_line = to_string(line);

			if (!Helper::stringContains(std_line, ":"))
			{
				newData.resetLoginData();
				continue;
			}

			if (Helper::stringContains(std_line, "Application") || Helper::stringContains(std_line, "Notes"))
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

				sendData(newData, PasswordManager::LoginDataFileType::TXT);
				newData.resetLoginData();
			}
		}
	}

	Windows::Foundation::IAsyncAction LoginDataManager::writeTextData([[maybe_unused]] const Windows::Storage::StorageFile& file, [[maybe_unused]] const Windows::Foundation::Collections::IVectorView<PasswordManager::LoginData>& data) const
	{
		throw hresult_not_implemented(L"not implemented yet");
	}

	void LoginDataManager::readCsvData(const Windows::Foundation::Collections::IVectorView<hstring>& file_text)
	{
		PasswordManager::LoginData newData = make<PasswordManager::implementation::LoginData>();

		for (const auto line : file_text)
		{
			processCsvLine(line, newData, PasswordManager::LoginDataFileType::CSV);
		}
	}

	Windows::Foundation::IAsyncAction LoginDataManager::writeCsvData(const Windows::Storage::StorageFile& file, const Windows::Foundation::Collections::IVectorView<PasswordManager::LoginData>& data) const
	{
		Windows::Foundation::Collections::IVector<hstring> lines = single_threaded_vector<hstring>();
		lines.Append(L"name,url,username,password");

		for (const PasswordManager::LoginData& iterator : data)
		{
			lines.Append(iterator.getDataAsString(PasswordManager::LoginDataFileType::CSV));
		}

		co_await Windows::Storage::FileIO::WriteLinesAsync(file, lines);
	}
	
	void LoginDataManager::processCsvLine(const hstring line, PasswordManager::LoginData& current_data, const PasswordManager::LoginDataFileType& data_type)
	{
		const std::string std_line = to_string(line);

		if (Helper::stringContains(std_line, "name,url,username,password"))
		{
			return;
		}

		constexpr unsigned int num_columns = 4u;
		unsigned int currentSeparatorIndex = 0u;

		for (unsigned int iterator = 0u; iterator <= num_columns; ++iterator)
		{
			const unsigned int nextSeparatorIndex = static_cast<unsigned int>(iterator == num_columns ? std_line.size() - 1 : std_line.find(',', currentSeparatorIndex));

			const hstring winrt_str = to_hstring(std_line.substr(currentSeparatorIndex, nextSeparatorIndex - currentSeparatorIndex));
			switch (iterator)
			{
				case 0u:
					current_data.Name(winrt_str);
					break;

				case 1u:
					current_data.Url(winrt_str);
					break;

				case 2u:
					current_data.Username(winrt_str);
					break;

				case 3u:
					current_data.Password(winrt_str);
					break;

				default:
					sendData(current_data, data_type);
					current_data.resetLoginData();
					currentSeparatorIndex = 0u;
					continue;
			}

			currentSeparatorIndex = nextSeparatorIndex + 1u;
		}
	}
}
