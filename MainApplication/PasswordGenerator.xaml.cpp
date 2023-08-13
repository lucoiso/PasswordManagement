// Author: Lucas Oliveira Vilas-Bôas
// Year: 2023
// Repository: https://github.com/lucoiso/PasswordManagement

#include "pch.h"

#include "PasswordGenerator.xaml.h"
#include "PasswordGenerator.g.cpp"

#include "Helpers/SettingsHelper.h"

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MainApplication::implementation
{
    PasswordGenerator::PasswordGenerator()
    {
        InitializeComponent();
    }

    void PasswordGenerator::ComponentLoaded([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] Microsoft::UI::Xaml::RoutedEventArgs const& args)
    {
        LUPASS_LOG_FUNCTION();

        InitializeSettingsProperties();
        m_initialized = true;
    }

    Windows::Foundation::IAsyncAction PasswordGenerator::BP_Generate_Click([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] Microsoft::UI::Xaml::RoutedEventArgs const& args)
    {
        LUPASS_LOG_FUNCTION();

        const hstring generated_password = co_await winrt::MainApplication::Generator::GeneratePassword();
        TB_Password().Text(generated_password);

        Helper::InsertSettingValue(SETTING_GENERATOR_LAST_GENERATED_PASSWORD, generated_password);
    }

    void PasswordGenerator::BP_Copy_Click([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] Microsoft::UI::Xaml::RoutedEventArgs const& args)
    {
        LUPASS_LOG_FUNCTION();

        Windows::ApplicationModel::DataTransfer::DataPackage data;
        data.SetText(TB_Password().Text());

        Windows::ApplicationModel::DataTransfer::Clipboard::SetContent(data);

        Controls::Flyout flyout;
        Controls::TextBlock info;
        info.Text(L"Copied to clipboard!");
        flyout.Content(info);
        flyout.ShowAt(BP_Copy());
    }

    void PasswordGenerator::BP_Close_Click([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] Microsoft::UI::Xaml::RoutedEventArgs const& args)
    {
        LUPASS_LOG_FUNCTION();

        m_close();
    }

    void PasswordGenerator::Generator_Data_Changed([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] Microsoft::UI::Xaml::RoutedEventArgs const& args)
    {
        if (!m_initialized)
        {
            return;
        }

        LUPASS_LOG_FUNCTION();

        const auto element = sender.as<Microsoft::UI::Xaml::FrameworkElement>();
        const auto tag = element.Tag().as<hstring>();

        if (Helper::StringEquals(tag, L"LowerCase_C"))
        {
            Helper::InsertSettingValue(SETTING_GENERATOR_ENABLE_LOWERCASE, CB_EnableLowerCase().IsChecked());
        }
        else if (Helper::StringEquals(tag, L"UpperCase_C"))
        {
            Helper::InsertSettingValue(SETTING_GENERATOR_ENABLE_UPPERCASE, CB_EnableUpperCase().IsChecked());
        }
        else if (Helper::StringEquals(tag, L"Numbers_C"))
        {
            Helper::InsertSettingValue(SETTING_GENERATOR_ENABLE_NUMBERS, CB_EnableNumbers().IsChecked());
        }
        else if (Helper::StringEquals(tag, L"CustomCharacters_C"))
        {
            Helper::InsertSettingValue(SETTING_GENERATOR_ENABLE_CUSTOM_CHARACTERS, CB_EnableCustomCharacters().IsChecked());
        }
        else if (Helper::StringEquals(tag, L"CustomCharacters_T"))
        {
            Helper::InsertSettingValue(SETTING_GENERATOR_CUSTOM_CHARACTERS, TB_CustomCharacters().Text());
        }
        else if (Helper::StringEquals(tag, L"Password_S"))
        {
            Helper::InsertSettingValue(SETTING_GENERATOR_PASSWORD_SIZE, static_cast<int>(SL_PasswordSize().Value()));
        }
    }

    void PasswordGenerator::InitializeSettingsProperties()
    {
        LUPASS_LOG_FUNCTION();

        TB_Password().Text(Helper::GetSettingValue<hstring>(SETTING_GENERATOR_LAST_GENERATED_PASSWORD));
        CB_EnableLowerCase().IsChecked(Helper::GetSettingValue<bool>(SETTING_GENERATOR_ENABLE_LOWERCASE));
        CB_EnableUpperCase().IsChecked(Helper::GetSettingValue<bool>(SETTING_GENERATOR_ENABLE_UPPERCASE));
        CB_EnableNumbers().IsChecked(Helper::GetSettingValue<bool>(SETTING_GENERATOR_ENABLE_NUMBERS));
        CB_EnableCustomCharacters().IsChecked(Helper::GetSettingValue<bool>(SETTING_GENERATOR_ENABLE_CUSTOM_CHARACTERS));
        SL_PasswordSize().Value(Helper::GetSettingValue<int>(SETTING_GENERATOR_PASSWORD_SIZE));
        TB_CustomCharacters().Text(Helper::GetSettingValue<hstring>(SETTING_GENERATOR_CUSTOM_CHARACTERS));
    }

    event_token PasswordGenerator::OnClose(MainApplication::SingleVoidDelegate const& handler)
    {
        LUPASS_LOG_FUNCTION();

        return m_close.add(handler);
    }

    void PasswordGenerator::OnClose(event_token const& token) noexcept
    {
        LUPASS_LOG_FUNCTION();

        m_close.remove(token);
    }
}