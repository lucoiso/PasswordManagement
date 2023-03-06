// Author: Lucas Oliveira Vilas-B�as
// Year: 2023
// Repository: https://github.com/lucoiso/PasswordManagement

#pragma once

#include "Constants.h"

#include <string>
#include <hstring.h>

namespace winrt::Helper
{
	template <typename T>
	inline constexpr bool SetMemberValue(const T& value, T& member)
	{
		if (value == member)
		{
			return false;
		}

		member = value;		
		return true;
	}

	template<typename ArgTy>
	inline constexpr const bool HasEmptyData(const ArgTy& arg)
	{
		return arg.empty();
	}

	template<typename ArgTy, typename ...Args>
	inline constexpr const bool HasEmptyData(const ArgTy& arg, Args&& ...args)
	{
		return arg.empty() || HasEmptyData(std::forward<Args>(args)...);
	}

	inline const bool StringContains(const std::string& string, const std::string& find, bool caseSensitive = false)
	{
		if (caseSensitive)
		{
			return string.rfind(find) != std::string::npos;
		}
		else
		{
			std::string stringLower = string;
			std::string findLower = find;

			const auto to_lower = [](const char& c) -> char
			{
				return static_cast<char>(std::tolower(c));
			};

			std::transform(stringLower.begin(), stringLower.end(), stringLower.begin(), to_lower);
			std::transform(findLower.begin(), findLower.end(), findLower.begin(), to_lower);

			return stringLower.rfind(findLower) != std::string::npos;
		}
	}

	inline const bool StringContains(const hstring& string, const hstring& find, bool caseSensitive = false)
	{
		return StringContains(to_string(string), to_string(find), caseSensitive);
	}

	inline void PrintDebugLine(const hstring& message)
	{
		if constexpr (ENABLE_DEBBUGGING)
		{
			OutputDebugStringW(message.c_str());
			OutputDebugStringW(L"\n");
		}
	}

	inline const hstring GetCleanUrlString(const hstring& value)
	{
		std::string new_url = to_string(value);
		
		if (new_url.empty())
		{
			return value;
		}

		const auto removal_lambda = [&new_url](const std::string_view& remove) {
			if (remove.empty())
			{
				return;
			}

			if (const auto find_index = new_url.rfind(remove);
				find_index != std::string::npos)
			{
				const auto removal_start = find_index + remove.size();
				new_url = new_url.substr(removal_start, new_url.size() - removal_start);
			}
		};

		removal_lambda("://");
		removal_lambda("www.");

		if (new_url.rfind("/", new_url.size() - 1) == new_url.size() - 1)
		{
			new_url = new_url.substr(0, new_url.size() - 1);
		}

		new_url.shrink_to_fit();

		return to_hstring(new_url);
	}
}