// Author: Lucas Oliveira Vilas-Bôas
// Year: 2023
// Repository: https://github.com/lucoiso/PasswordManagement

#pragma once

#include "BackupDataListItem.g.h"

namespace winrt::MainApplication::implementation
{
    struct BackupDataListItem : BackupDataListItemT<BackupDataListItem>
    {
        BackupDataListItem();

        Windows::Foundation::IAsyncAction BT_Restore_Update_Click(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args);
        Windows::Foundation::IAsyncAction BT_Restore_Replace_Click(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args);

        uint64_t BackupTime() const;
        void BackupTime(uint64_t value);

        hstring Date();

        event_token PropertyChanged(Microsoft::UI::Xaml::Data::PropertyChangedEventHandler const& value);
        void PropertyChanged(event_token const& token);

    private:
        uint64_t m_backup_time;
        event<Microsoft::UI::Xaml::Data::PropertyChangedEventHandler> m_property_changed;
    };
}

namespace winrt::MainApplication::factory_implementation
{
    struct BackupDataListItem : BackupDataListItemT<BackupDataListItem, implementation::BackupDataListItem>
    {
    };
}
