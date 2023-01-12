// Author: Lucas Oliveira Vilas-Bôas
// Year: 2022
// Repository:

#include "pch.h"

#include "LoginDataTypeComboBox.xaml.h"
#include "LoginDataTypeComboBox.g.cpp"

using namespace winrt;
using namespace Microsoft::UI::Xaml;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::MainApplication::implementation
{
    LoginDataTypeComboBox::LoginDataTypeComboBox()
    {
        InitializeComponent();

        this->m_options = single_threaded_observable_vector<hstring>();

        this->m_options.Append(L"Kapersky");
        this->m_options.Append(L"Google");
        this->m_options.Append(L"Microsoft");
    }

    winrt::Windows::Foundation::Collections::IObservableVector<hstring> LoginDataTypeComboBox::Options() const
    {
        return this->m_options;
    }

    hstring LoginDataTypeComboBox::SelectedOption() const
    {
        return this->m_selectedoption;
    }

    void LoginDataTypeComboBox::CB_Options_SelectionChanged([[maybe_unused]] winrt::Windows::Foundation::IInspectable const& sender, [[maybe_unused]] winrt::Microsoft::UI::Xaml::RoutedEventArgs const& args)
    {
        this->m_selectedoption = unbox_value<hstring>(CB_Options().SelectedValue());
    }
}
