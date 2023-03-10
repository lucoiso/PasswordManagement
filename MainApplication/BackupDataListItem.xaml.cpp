// Author: Lucas Oliveira Vilas-B�as
// Year: 2023
// Repository: https://github.com/lucoiso/PasswordManagement

#include "pch.h"

#include "BackupDataListItem.xaml.h"
#include "BackupDataListItem.g.cpp"

#include "DialogManager.h"

#include "Helpers/SecurityHelper.h"
#include "Helpers/CastingHelper.h"

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MainApplication::implementation
{
    BackupDataListItem::BackupDataListItem()
    {
        InitializeComponent();
    }

    Windows::Foundation::IAsyncAction BackupDataListItem::BT_Restore_Update_Click(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args)
    {
        LUPASS_LOG_FUNCTION();

        if (!(co_await Helper::RequestUserCredentials(XamlRoot())))
        {
            co_return;
        }

        const auto result = co_await DialogManager::GetInstance().ShowDialogAsync(XamlRoot(), L"Restore Backup (Update)", L"Will add all backup data and update existing informations. Confirm Process?", true, true, L"Confirm", L"Cancel");

        switch (result)
        {
            case Microsoft::UI::Xaml::Controls::ContentDialogResult::Primary:
                if (auto MainPage = Helper::GetParent<MainApplication::MainPage>(*this); MainPage)
                {
                    DialogManager::GetInstance().ShowLoadingDialog(XamlRoot());

                    try
                    {
                        // Import from backup file
                    }
                    catch (const hresult_error& e)
                    {
                        DialogManager::GetInstance().HideLoadingDialog();
                        Helper::PrintDebugLine(e.message());
                    }

                    DialogManager::GetInstance().HideLoadingDialog();
                }

                break;

            default: break;
        }
    }

    Windows::Foundation::IAsyncAction BackupDataListItem::BT_Restore_Replace_Click(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args)
    {
        LUPASS_LOG_FUNCTION();

        if (!(co_await Helper::RequestUserCredentials(XamlRoot())))
        {
            co_return;
        }

        const auto result = co_await DialogManager::GetInstance().ShowDialogAsync(XamlRoot(), L"Restore Backup (Replace)", L" Will replace all current data with the backup data, removing existing informations that doesn't exists in the backup data. Confirm process?", true, true, L"Confirm", L"Cancel");

        switch (result)
        {
            case Microsoft::UI::Xaml::Controls::ContentDialogResult::Primary:
                if (auto MainPage = Helper::GetParent<MainApplication::MainPage>(*this); MainPage)
                {
                    DialogManager::GetInstance().ShowLoadingDialog(XamlRoot());

                    try
                    {
                        // Clear all existing data and import from backup file
                    }
                    catch (const hresult_error& e)
                    {
                        DialogManager::GetInstance().HideLoadingDialog();
                        Helper::PrintDebugLine(e.message());
                    }

                    DialogManager::GetInstance().HideLoadingDialog();
                }

                break;

            default: break;
        }
    }

    uint64_t BackupDataListItem::BackupTime() const
    {
        return m_backup_time;
    }

    void BackupDataListItem::BackupTime(uint64_t value)
    {
        if (Helper::SetMemberValue(value, m_backup_time))
        {
            m_property_changed(*this, Data::PropertyChangedEventArgs{ L"Date" });
        }
    }

    hstring BackupDataListItem::Date() const
    {
        return Helper::TimeToString(Helper::ToTimePoint(BackupTime()), true, false);
    }

    event_token BackupDataListItem::PropertyChanged(Microsoft::UI::Xaml::Data::PropertyChangedEventHandler const& value)
    {
        return m_property_changed.add(value);
    }

    void BackupDataListItem::PropertyChanged(event_token const& token)
    {
        m_property_changed.remove(token);
    }
}
