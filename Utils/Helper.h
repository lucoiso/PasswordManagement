// Author: Lucas Oliveira Vilas-Bôas
// Year: 2022
// Repository: https://github.com/lucoiso/PasswordManagement

#pragma once

#include "Constants.h"

#include <string>
#include <hstring.h>

namespace winrt::Helper
{
	template <typename T>
	inline constexpr bool setMemberValue(const T& value, T& member)
	{
		if (value == member)
		{
			return false;
		}

		member = value;		
		return true;
	}

	template<typename ArgTy>
	inline constexpr const bool hasEmptyData(const ArgTy& arg)
	{
		return arg.empty();
	}

	template<typename ArgTy, typename ...Args>
	inline constexpr const bool hasEmptyData(const ArgTy& arg, Args&& ...args)
	{
		return arg.empty() || hasEmptyData(std::forward<Args>(args)...);
	}

	inline const bool stringContains(const std::string& string, const std::string& find)
	{
		return string.find(find) != std::string::npos;
	}

	inline const bool stringContains(const hstring& string, const hstring& find)
	{
		return stringContains(to_string(string), to_string(find));
	}

	inline void printDebugLine(const hstring& message)
	{
		if constexpr (ALLOW_DEBBUGGING)
		{
			OutputDebugStringW(message.c_str());
			OutputDebugStringW(L"\n");
		}
	}
}