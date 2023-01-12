// Author: Lucas Oliveira Vilas-Bôas
// Year: 2022
// Repository: https://github.com/lucoiso/PasswordManagement

#pragma once

#include <string>

namespace Helper
{
	template <typename T>
	inline constexpr void setMemberValue(const T& value, T& member)
	{
		if (value == member)
		{
			return;
		}

		member = value;
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
}