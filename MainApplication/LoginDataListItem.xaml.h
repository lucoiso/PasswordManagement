// Author: Lucas Oliveira Vilas-Bôas
// Year: 2023
// Repository: https://github.com/lucoiso/PasswordManagement

#pragma once

#include "LoginDataListItem.g.h"

namespace winrt::MainApplication::implementation
{
    struct LoginDataListItem : LoginDataListItemT<LoginDataListItem>
    {
        LoginDataListItem();

        bool EnableLicenseTools() const;

        PasswordManager::LoginData Data() const;
        void Data(PasswordManager::LoginData const& value);

        Windows::Foundation::IAsyncAction BT_TogglePassword_Clicked(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args);
        Windows::Foundation::IAsyncAction BT_CopyContent_Clicked(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args);

        Windows::Foundation::IAsyncAction BT_Edit_Clicked(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args);
        Windows::Foundation::IAsyncAction BT_Delete_Clicked(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args);

        void EmitUsedEvent(const bool update_value = true);
        void EmitChangedEvent(const bool update_value = true);

        hstring TimeCreated() const;
        hstring TimeUsed() const;
        hstring TimeChanged() const;

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
