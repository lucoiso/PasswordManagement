// Author: Lucas Oliveira Vilas-Bôas
// Year: 2022
// Repository: https://github.com/lucoiso/PasswordManagement

#pragma once

#include "LoginDataListItem.g.h"

namespace winrt::MainApplication::implementation
{
    struct LoginDataListItem : LoginDataListItemT<LoginDataListItem>
    {
        LoginDataListItem();

        PasswordManager::LoginData Data() const;
        void Data(PasswordManager::LoginData const& value);

        void BT_TogglePassword_Clicked(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args);

        hstring Password() const;

        event_token PropertyChanged(Microsoft::UI::Xaml::Data::PropertyChangedEventHandler const& value);
        void PropertyChanged(event_token const& token);
        
    private:
		PasswordManager::LoginData m_data;
        bool m_show_password{ false };
        event<Microsoft::UI::Xaml::Data::PropertyChangedEventHandler> m_property_changed;
    };
}

namespace winrt::MainApplication::factory_implementation
{
    struct LoginDataListItem : LoginDataListItemT<LoginDataListItem, implementation::LoginDataListItem>
    {
    };
}
