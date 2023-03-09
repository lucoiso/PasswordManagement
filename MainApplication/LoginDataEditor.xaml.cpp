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

    void LoginDataEditor::TB_Name_TextChanged([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] Microsoft::UI::Xaml::Controls::TextChangedEventArgs const& args)
    {
        LUPASS_LOG_FUNCTION();

        m_data.Name(TB_Name().Text());
    }

    void LoginDataEditor::TB_Url_TextChanged([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] Microsoft::UI::Xaml::Controls::TextChangedEventArgs const& args)
    {
        LUPASS_LOG_FUNCTION();

        m_data.Url(TB_Url().Text());
    }

    void LoginDataEditor::TB_Username_TextChanged([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] Microsoft::UI::Xaml::Controls::TextChangedEventArgs const& args)
    {
        LUPASS_LOG_FUNCTION();

        m_data.Username(TB_Username().Text());
    }

    void LoginDataEditor::TB_Notes_TextChanged([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] Microsoft::UI::Xaml::Controls::TextChangedEventArgs const& args)
    {
        LUPASS_LOG_FUNCTION();

        m_data.Notes(TB_Notes().Text());
    }

    void LoginDataEditor::TB_Password_PasswordChanged([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] Microsoft::UI::Xaml::RoutedEventArgs const& args)
    {
        LUPASS_LOG_FUNCTION();

        m_data.Password(TB_Password().Password());
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
