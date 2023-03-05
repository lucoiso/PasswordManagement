// Author: Lucas Oliveira Vilas-Bôas
// Year: 2023
// Repository: https://github.com/lucoiso/PasswordManagement

#include "pch.h"

#include "Generator.h"
#include "Generator.g.cpp"

#include <random>
#include <ctime>
#include <thread>
#include <future>

#include "Constants.h"

using namespace winrt;

namespace winrt::PasswordGenerator::implementation
{
	hstring GeneratePasswordInBackground()
	{
		const auto local_settings = Windows::Storage::ApplicationData::Current().LocalSettings();
		const bool enable_lowercase = local_settings.Values().TryLookup(SETTING_GENERATOR_ENABLE_LOWERCASE).as<bool>();
		const bool enable_uppercase = local_settings.Values().TryLookup(SETTING_GENERATOR_ENABLE_UPPERCASE).as<bool>();
		const bool enable_numbers = local_settings.Values().TryLookup(SETTING_GENERATOR_ENABLE_NUMBERS).as<bool>();
		const bool enable_custom_characters = local_settings.Values().TryLookup(SETTING_GENERATOR_ENABLE_CUSTOM_CHARACTERS).as<bool>();
		const hstring custom_characters = local_settings.Values().TryLookup(SETTING_GENERATOR_CUSTOM_CHARACTERS).as<hstring>();
		const int password_size = local_settings.Values().TryLookup(SETTING_GENERATOR_PASSWORD_SIZE).as<int>();
		const hstring previous_password = local_settings.Values().TryLookup(SETTING_GENERATOR_LAST_GENERATED_PASSWORD).as<hstring>();

		std::wstring allowed_characters;
		if (enable_lowercase)
		{
			allowed_characters += L"abcdefghijklmnopqrstuvwxyz";
		}
		if (enable_uppercase)
		{
			allowed_characters += L"ABCDEFGHIJKLMNOPQRSTUVWXYZ";
		}
		if (enable_numbers)
		{
			allowed_characters += L"0123456789";
		}
		if (enable_custom_characters)
		{
			allowed_characters += custom_characters;
		}

		if (allowed_characters.empty())
		{
			return L"Invalid";
		}

		std::wstring new_password;

		bool same_pass = true;
		while (same_pass)
		{
			new_password = L"";
			std::mt19937 random_engine(static_cast<unsigned int>(std::time(nullptr)));
			std::uniform_int_distribution<size_t> random_index(0, allowed_characters.size() - 1);
			for (int i = 0; i < password_size; i++)
			{
				new_password += allowed_characters[random_index(random_engine)];
			}

			same_pass = (new_password == previous_password);
		}

		return to_hstring(to_string(new_password));
	}

	Windows::Foundation::IAsyncOperation<hstring> Generator::GeneratePassword()
	{
		std::future<hstring> password_future = std::async(std::launch::async, GeneratePasswordInBackground);
		return co_await password_future;
	}
}
