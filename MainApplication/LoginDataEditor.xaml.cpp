// Author: Lucas Oliveira Vilas-Bôas
// Year: 2023
// Repository: https://github.com/lucoiso/PasswordManagement

#include "pch.h"

#include "LoginDataEditor.xaml.h"
#include "LoginDataEditor.g.cpp"

#include "PasswordGenerator.xaml.h"

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MainApplication::implementation
{
    LoginDataEditor::LoginDataEditor()
    {
        InitializeComponent();

        Style(Application::Current().Resources().Lookup(box_value(L"DefaultContentDialogStyle")).as<Microsoft::UI::Xaml::Style>());
        DefaultButton(Microsoft::UI::Xaml::Controls::ContentDialogButton::Primary);
    }

    PasswordManager::LoginData MainApplication::implementation::LoginDataEditor::Data() const
    {
        return m_data;
    }

    void MainApplication::implementation::LoginDataEditor::Data(PasswordManager::LoginData const& value)
    {
        Helper::SetMemberValue(value, m_data);

        TB_Name().Text(m_data.Name());
        TB_Url().Text(m_data.Url());
        TB_Username().Text(m_data.Username());
        TB_Notes().Text(m_data.Notes());
        TB_Password().Password(m_data.Password());
    }

    void LoginDataEditor::TB_Data_Changed([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] Microsoft::UI::Xaml::RoutedEventArgs const& args)
    {
        LUPASS_LOG_FUNCTION();

        const auto element = sender.as<Microsoft::UI::Xaml::FrameworkElement>();
        const auto tag = element.Tag().as<hstring>();

        if (Helper::StringEquals(tag, L"Name"))
        {
            m_data.Name(TB_Name().Text());
        }
        else if (Helper::StringEquals(tag, L"Url"))
        {
            m_data.Url(TB_Url().Text());
        }
        else if (Helper::StringEquals(tag, L"Username"))
        {
            m_data.Username(TB_Username().Text());
        }
        else if (Helper::StringEquals(tag, L"Notes"))
        {
            m_data.Notes(TB_Notes().Text());
        }
        else if (Helper::StringEquals(tag, L"Password"))
        {
            m_data.Password(TB_Password().Password());
        }
    }

    void LoginDataEditor::BT_TogglePasswordView_Click([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] Microsoft::UI::Xaml::RoutedEventArgs const& args)
    {
        if (TB_Password().PasswordRevealMode() == Microsoft::UI::Xaml::Controls::PasswordRevealMode::Hidden)
        {
            TB_Password().PasswordRevealMode(Microsoft::UI::Xaml::Controls::PasswordRevealMode::Visible);
            FI_TogglePassword().Glyph(L"\xE9A8");
        }
        else
        {
            TB_Password().PasswordRevealMode(Microsoft::UI::Xaml::Controls::PasswordRevealMode::Hidden);
            FI_TogglePassword().Glyph(L"\xE9A9");
        }
    }

    void LoginDataEditor::BT_GeneratePassword_Click([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] Microsoft::UI::Xaml::RoutedEventArgs const& args)
    {
        LUPASS_LOG_FUNCTION();

        Controls::Flyout flyout;        
        auto generator_content = winrt::make<MainApplication::implementation::PasswordGenerator>();

        generator_content.OnClose(
            [flyout]()
            {
                flyout.Hide();
            }
        );

        flyout.Content(generator_content);
        flyout.ShowAt(BP_GeneratePassword());
    }
}
