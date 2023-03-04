// Author: Lucas Oliveira Vilas-Bôas
// Year: 2023
// Repository: https://github.com/lucoiso/PasswordManagement

#include "pch.h"

#include "LoginDataEditor.xaml.h"
#include "LoginDataEditor.g.cpp"

#include <Helper.h>

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
        TB_Password().Password(m_data.Password());
    }

    void LoginDataEditor::TB_Name_TextChanged([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] Microsoft::UI::Xaml::Controls::TextChangedEventArgs const& args)
    {
        m_data.Name(TB_Name().Text());
    }

    void LoginDataEditor::TB_Url_TextChanged([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] Microsoft::UI::Xaml::Controls::TextChangedEventArgs const& args)
    {
        m_data.Url(TB_Url().Text());
    }

    void LoginDataEditor::TB_Username_TextChanged([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] Microsoft::UI::Xaml::Controls::TextChangedEventArgs const& args)
    {
        m_data.Username(TB_Username().Text());
    }

    void LoginDataEditor::TB_Password_PasswordChanged([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] Microsoft::UI::Xaml::RoutedEventArgs const& args)
    {
        m_data.Password(TB_Password().Password());
    }
}
