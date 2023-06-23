// Author: Lucas Oliveira Vilas-Bôas
// Year: 2023
// Repository: https://github.com/lucoiso/PasswordManagement

#include "pch.h"

#include "LoginDataListItem.xaml.h"
#include "LoginDataListItem.g.cpp"

#include "DialogManager.h"
#include "DataManager.h"

#include "Helpers/SecurityHelper.h"
#include "Helpers/CastingHelper.h"

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MainApplication::implementation
{
    LoginDataListItem::LoginDataListItem()
    {
        InitializeComponent();
    }

    void LoginDataListItem::ComponentLoaded([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] Microsoft::UI::Xaml::RoutedEventArgs const& args)
    {
        if (!Helper::GetSettingValue<bool>(LICENSING_ENABLED_KEY))
        {
            BT_Edit().IsEnabled(false);
		}
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

            m_show_password = false;
            m_property_changed(*this, Data::PropertyChangedEventArgs{ L"Password" });

            m_property_changed(*this, Data::PropertyChangedEventArgs{ L"TimeCreated" });
            m_property_changed(*this, Data::PropertyChangedEventArgs{ L"TimeUsed" });
            m_property_changed(*this, Data::PropertyChangedEventArgs{ L"TimeChanged" });
        }
    }

    Windows::Foundation::IAsyncAction MainApplication::implementation::LoginDataListItem::BT_TogglePassword_Clicked([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] RoutedEventArgs const& args)
    {
        LUPASS_LOG_FUNCTION();

        if (!(co_await Helper::RequestUserCredentials()))
        {
			co_return;
		}

        m_show_password = !m_show_password;
        m_property_changed(*this, Data::PropertyChangedEventArgs{ L"Password" });

        FI_TogglePassword().Glyph(m_show_password ? L"\xE9A8" : L"\xE9A9");

        EmitUsedEvent();
    }

    void CopyContentToClipboard(const hstring& content)
    {
        LUPASS_LOG_FUNCTION();

        Windows::ApplicationModel::DataTransfer::DataPackage data;
        data.SetText(content);

        Windows::ApplicationModel::DataTransfer::Clipboard::SetContent(data);
    }

    Windows::Foundation::IAsyncAction LoginDataListItem::BT_CopyContent_Clicked(Windows::Foundation::IInspectable const& sender, [[maybe_unused]] RoutedEventArgs const& args)
    {
        LUPASS_LOG_FUNCTION();

		const auto element = sender.as<Microsoft::UI::Xaml::FrameworkElement>();
		const auto tag = element.Tag().as<hstring>();

		if (Helper::StringEquals(tag, L"Username"))
		{
			CopyContentToClipboard(m_data.Username());
		}
		else if (Helper::StringEquals(tag, L"Password"))
		{
            if (!(co_await Helper::RequestUserCredentials()))
            {
                co_return;
            }

			CopyContentToClipboard(m_data.Password());
		}
		else if (Helper::StringEquals(tag, L"Url"))
		{
			CopyContentToClipboard(m_data.Url());
		}

		Controls::Flyout flyout;
        Controls::TextBlock info;
        info.Text(L"Copied to clipboard!");
		flyout.Content(info);
        flyout.ShowAt(element);

        EmitUsedEvent();
    }

    Windows::Foundation::IAsyncAction LoginDataListItem::BT_Edit_Clicked(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args)
    {
        LUPASS_LOG_FUNCTION();

        if (!(co_await Helper::RequestUserCredentials()))
        {
            co_return;
        }

        MainApplication::LoginDataEditor editor;
        editor.XamlRoot(DialogManager::GetInstance().GetMainWindowContentXamlRoot());
        editor.Title(box_value(L"Editor"));
        editor.Data(Data().Clone().as<PasswordManager::LoginData>());

        switch ((co_await editor.ShowAsync()))
        {
            case Microsoft::UI::Xaml::Controls::ContentDialogResult::Primary:
            {
                if (editor.Data().HasEmptyData())
                {
                    co_await DialogManager::GetInstance().ShowDialogAsync(L"Error", L"Registered data contains empty values.", false, true);
                }
                else
                {
                    EmitChangedEvent();

                    const auto new_data = editor.Data().Clone().as<PasswordManager::LoginData>();
                    new_data.Changed(Data().Changed());

                    const auto current_data = Data().Clone().as<PasswordManager::LoginData>();

                    const bool is_new_data = !current_data.Equals(new_data);
                    if (is_new_data)
                    {
                        co_await DataManager::GetInstance().RemoveLoginDataAsync({ Data() }, false);
                    }

                    co_await DataManager::GetInstance().InsertLoginDataAsync({ new_data }, true);
                }

                break;
            }

            default: break;
        }
    }

    Windows::Foundation::IAsyncAction LoginDataListItem::BT_Delete_Clicked(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args)
    {
        LUPASS_LOG_FUNCTION();

        if (!(co_await Helper::RequestUserCredentials()))
        {
            co_return;
        }

        const auto result = co_await DialogManager::GetInstance().ShowDialogAsync(L"Delete Data", L"Confirm process?", true, true, L"Confirm", L"Cancel");

        switch (result)
        {
            case Microsoft::UI::Xaml::Controls::ContentDialogResult::Primary:
                co_await DataManager::GetInstance().RemoveLoginDataAsync({ Data() }, true);
                break;

            default: break;
        }
    }

    void LoginDataListItem::EmitUsedEvent(const bool update_value)
    {
        if (update_value)
        {
            m_data.Used(winrt::clock::now().time_since_epoch().count());
        }

        m_property_changed(*this, Data::PropertyChangedEventArgs{ L"TimeUsed" });
    }

    void LoginDataListItem::EmitChangedEvent(const bool update_value)
    {
        if (update_value)
        {
            m_data.Changed(winrt::clock::now().time_since_epoch().count());
        }

        m_property_changed(*this, Data::PropertyChangedEventArgs{ L"TimeChanged" });
    }

    hstring LoginDataListItem::TimeCreated() const
    {
        return Helper::TimeToString(Helper::ToTimePoint(m_data.Created()));
    }

    hstring LoginDataListItem::TimeUsed() const
    {
        return Helper::TimeToString(Helper::ToTimePoint(m_data.Used()));
    }

    hstring LoginDataListItem::TimeChanged() const
    {
        return Helper::TimeToString(Helper::ToTimePoint(m_data.Changed()));
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