// Author: Lucas Oliveira Vilas-Bôas
// Year: 2022
// Repository: https://github.com/lucoiso/PasswordManagement

#include "pch.h"

#include "LoginDataListItem.xaml.h"
#include "LoginDataListItem.g.cpp"

#include <Helper.h>

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MainApplication::implementation
{
    LoginDataListItem::LoginDataListItem()
    {
        InitializeComponent();
    }

    PasswordManager::LoginData MainApplication::implementation::LoginDataListItem::Data() const
    {
        return m_data;
    }

    void MainApplication::implementation::LoginDataListItem::Data(PasswordManager::LoginData const& value)
    {
        if (Helper::SetMemberValue(value, m_data))
        {
            m_property_changed(*this, Data::PropertyChangedEventArgs{ L"Data" });
        }
    }

    void MainApplication::implementation::LoginDataListItem::BT_TogglePassword_Clicked([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] RoutedEventArgs const& args)
    {
        m_show_password = !m_show_password;
        m_property_changed(*this, Data::PropertyChangedEventArgs{ L"Password" });
    }

    hstring MainApplication::implementation::LoginDataListItem::Password() const
    {
        return m_show_password ? m_data.Password() : L"****************";
    }
    
    event_token MainApplication::implementation::LoginDataListItem::PropertyChanged(Microsoft::UI::Xaml::Data::PropertyChangedEventHandler const& value)
    {
        return m_property_changed.add(value);
    }
    
    void MainApplication::implementation::LoginDataListItem::PropertyChanged(event_token const& token)
    {
		m_property_changed.remove(token);
    }
}
