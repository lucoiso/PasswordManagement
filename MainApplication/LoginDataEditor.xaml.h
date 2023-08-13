// Author: Lucas Oliveira Vilas-Bôas
// Year: 2023
// Repository: https://github.com/lucoiso/PasswordManagement

#pragma once

#include "LoginDataEditor.g.h"
#include "LoginData.h"

namespace winrt::MainApplication::implementation
{
    struct LoginDataEditor : LoginDataEditorT<LoginDataEditor>
    {
        LoginDataEditor();

        MainApplication::LoginData Data() const;
        void Data(MainApplication::LoginData const& value);

        void TB_Data_Changed(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args);

        void BT_TogglePasswordView_Click(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args);
        void BT_GeneratePassword_Click(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args);

    private:
        MainApplication::LoginData m_data;
    };
}

namespace winrt::MainApplication::factory_implementation
{
    struct LoginDataEditor : LoginDataEditorT<LoginDataEditor, implementation::LoginDataEditor>
    {
    };
}
