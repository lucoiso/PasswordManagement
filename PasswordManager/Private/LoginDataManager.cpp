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

namespace winrt::PasswordManager::implementation
{
	winrt::Windows::Foundation::IAsyncAction LoginDataManager::importData(const winrt::Windows::Storage::StorageFile& file)
	{
		if (!file.IsAvailable())
		{
			throw hresult_invalid_argument(L"file is unavailable");
		}
		const hstring fileType = file.FileType();
		const auto fileContent = (co_await winrt::Windows::Storage::FileIO::ReadLinesAsync(file)).GetView();

		if (fileType == L".txt")
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

	winrt::Windows::Foundation::IAsyncAction LoginDataManager::exportData(const winrt::Windows::Storage::StorageFile& file, const winrt::Windows::Foundation::Collections::IVectorView<winrt::PasswordManager::LoginData>& data)
	{
		if (!file.IsAvailable())
		{
			throw hresult_invalid_argument(L"file is unavailable");
		}

		const hstring fileType = file.FileType();

		if (fileType == L".txt")
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

	event_token LoginDataManager::LoginDataUpdated(winrt::Windows::Foundation::EventHandler<PasswordManager::LoginUpdateEventParams> const& handler)
	{
		return this->m_data_updated.add(handler);
	}

	void LoginDataManager::LoginDataUpdated(event_token const& token) noexcept
	{
		this->m_data_updated.remove(token);
	}

	void LoginDataManager::sendData(const winrt::PasswordManager::LoginData& data, const winrt::PasswordManager::LoginDataFileType& type)
	{
		this->m_data_updated(*this, winrt::make<winrt::PasswordManager::implementation::LoginUpdateEventParams>(data, type));
	}

	void LoginDataManager::readTextData(const winrt::Windows::Foundation::Collections::IVectorView<hstring>& file_text)
	{
		winrt::PasswordManager::LoginData newData = make<winrt::PasswordManager::implementation::LoginData>();

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

				sendData(newData, winrt::PasswordManager::LoginDataFileType::TXT);
				newData.resetLoginData();
			}
		}
	}

	winrt::Windows::Foundation::IAsyncAction LoginDataManager::writeTextData([[maybe_unused]] const winrt::Windows::Storage::StorageFile& file, [[maybe_unused]] const winrt::Windows::Foundation::Collections::IVectorView<winrt::PasswordManager::LoginData>& data) const
	{
		throw hresult_not_implemented(L"not implemented yet");
	}

	void LoginDataManager::readCsvData(const winrt::Windows::Foundation::Collections::IVectorView<hstring>& file_text)
	{
		winrt::PasswordManager::LoginData newData = make<winrt::PasswordManager::implementation::LoginData>();

		for (const auto line : file_text)
		{
			const std::string std_line = to_string(line);

			if (Helper::stringContains(std_line, "name,url,username,password"))
			{
				continue;
			}

			constexpr unsigned int num_columns = 4u;
			unsigned int currentSeparatorIndex = 0u;

			for (unsigned int iterator = 0u; iterator <= num_columns; ++iterator)
			{
				const unsigned int nextSeparatorIndex = static_cast<unsigned int>(iterator == num_columns ? std_line.size() - 1 : std_line.find(',', currentSeparatorIndex));

				const hstring value_hstr = to_hstring(std_line.substr(currentSeparatorIndex, nextSeparatorIndex - currentSeparatorIndex));
				switch (iterator)
				{
					case 0u:
						newData.Name(value_hstr);
						break;

					case 1u:
						newData.Url(value_hstr);
						break;

					case 2u:
						newData.Username(value_hstr);
						break;

					case 3u:
						newData.Password(value_hstr);
						break;

					default:
						sendData(newData, winrt::PasswordManager::LoginDataFileType::TXT);
						newData.resetLoginData();
						currentSeparatorIndex = 0u;
						continue;
				}

				currentSeparatorIndex = nextSeparatorIndex + 1u;
			}
		}
	}

	winrt::Windows::Foundation::IAsyncAction LoginDataManager::writeCsvData(const winrt::Windows::Storage::StorageFile& file, const winrt::Windows::Foundation::Collections::IVectorView<winrt::PasswordManager::LoginData>& data) const
	{
		winrt::Windows::Foundation::Collections::IVector<hstring> lines = winrt::single_threaded_vector<hstring>();
		lines.Append(L"name,url,username,password");

		for (const winrt::PasswordManager::LoginData& iterator : data)
		{
			lines.Append(iterator.getDataAsString(winrt::PasswordManager::LoginDataFileType::CSV));
		}

		co_await winrt::Windows::Storage::FileIO::WriteLinesAsync(file, lines);
	}
}
