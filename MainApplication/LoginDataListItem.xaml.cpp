// Author: Lucas Oliveira Vilas-Bôas
// Year: 2023
// Repository: https://github.com/lucoiso/PasswordManagement

#include "pch.h"

#include "LoginDataListItem.xaml.h"
#include "LoginDataListItem.g.cpp"

#include "MainPage.xaml.h"

#include "SecurityHelper.h"
#include "CastingHelper.h"
#include "DialogHelper.h"

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MainApplication::implementation
{
    LoginDataListItem::LoginDataListItem()
    {
        InitializeComponent();
    }

    bool LoginDataListItem::EnableLicenseTools() const
    {
        return Helper::GetParent<MainApplication::MainPage>(*this).EnableLicenseTools();
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
        }
    }

    Windows::Foundation::IAsyncAction MainApplication::implementation::LoginDataListItem::BT_TogglePassword_Clicked([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] RoutedEventArgs const& args)
    {
        LUPASS_LOG_FUNCTION();

        if (!(co_await Helper::RequestUserCredentials(XamlRoot())))
        {
			co_return;
		}

        m_show_password = !m_show_password;
        m_property_changed(*this, Data::PropertyChangedEventArgs{ L"Password" });

        FI_TogglePassword().Glyph(m_show_password ? L"\xE9A8" : L"\xE9A9");
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

		const auto button = sender.as<Microsoft::UI::Xaml::Controls::Button>();
		const auto tag = button.Tag().as<hstring>();

		if (tag == L"Username")
		{
			CopyContentToClipboard(m_data.Username());
		}
		else if (tag == L"Password")
		{
            if (!(co_await Helper::RequestUserCredentials(XamlRoot())))
            {
                co_return;
            }

			CopyContentToClipboard(m_data.Password());
		}
		else if (tag == L"Url")
		{
			CopyContentToClipboard(m_data.Url());
		}
        
		Controls::Flyout flyout;
        Controls::TextBlock info;
        info.Text(L"Copied to clipboard!");
		flyout.Content(info);
        flyout.ShowAt(button);
    }

    Windows::Foundation::IAsyncAction LoginDataListItem::BT_Edit_Clicked(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args)
    {
        LUPASS_LOG_FUNCTION();

        if (!(co_await Helper::RequestUserCredentials(XamlRoot())))
        {
            co_return;
        }

        MainApplication::LoginDataEditor editor;
        editor.XamlRoot(XamlRoot());
        editor.Data(Data());

        switch ((co_await editor.ShowAsync()))
        {
            case Microsoft::UI::Xaml::Controls::ContentDialogResult::Primary:
            {
                if (editor.Data().HasEmptyData())
                {
                    co_await Helper::CreateContentDialog(XamlRoot(), L"Error", L"Registered data contains empty values.", false, true).ShowAsync();
                }
                else
                {
                    Data(editor.Data().Clone().as<PasswordManager::LoginData>());
                    if (auto MainPage = Helper::GetParent<MainApplication::MainPage>(*this); MainPage)
                    {
                        co_await MainPage.SaveLocalDataAsync();
                    }
                }

                break;
            }

            default: co_return;
        }
    }

    Windows::Foundation::IAsyncAction LoginDataListItem::BT_Delete_Clicked(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args)
    {
        LUPASS_LOG_FUNCTION();

        if (!(co_await Helper::RequestUserCredentials(XamlRoot())))
        {
            co_return;
        }

        auto confirm_dialog = Helper::CreateContentDialog(XamlRoot(), L"Delete Data", L"Confirm process?", true, true);        

        switch ((co_await confirm_dialog.ShowAsync()))
        {
            case Microsoft::UI::Xaml::Controls::ContentDialogResult::Primary:
            {
                if (auto MainPage = Helper::GetParent<MainApplication::MainPage>(*this); MainPage)
                {
                    MainPage.RemoveLoginData(Data().Clone().as<PasswordManager::LoginData>());
                }
            }
        }
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
