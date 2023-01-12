// Author: Lucas Oliveira Vilas-Bôas
// Year: 2022
// Repository: https://github.com/lucoiso/PasswordManagement

#pragma once

#include "winrt/Microsoft.UI.Xaml.h"
#include "winrt/Microsoft.UI.Xaml.Markup.h"
#include "winrt/Microsoft.UI.Xaml.Controls.Primitives.h"
#include "LoginDataTypeComboBox.g.h"

namespace winrt::MainApplication::implementation
{
    struct LoginDataTypeComboBox : LoginDataTypeComboBoxT<LoginDataTypeComboBox>
    {
        LoginDataTypeComboBox();

        winrt::Windows::Foundation::Collections::IObservableVector<hstring> Options() const;
        hstring SelectedOption() const;

        void CB_Options_SelectionChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& args);

    private:
        hstring m_selectedoption;
        winrt::Windows::Foundation::Collections::IObservableVector<hstring> m_options;
    };
}

namespace winrt::MainApplication::factory_implementation
{
    struct LoginDataTypeComboBox : LoginDataTypeComboBoxT<LoginDataTypeComboBox, implementation::LoginDataTypeComboBox>
    {
    };
}
