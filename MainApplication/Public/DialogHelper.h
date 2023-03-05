// Author: Lucas Oliveira Vilas-Bôas
// Year: 2023
// Repository: https://github.com/lucoiso/PasswordManagement

#pragma once

#include "pch.h"
#include "ApplicationSettings.xaml.h"
#include "PasswordGenerator.xaml.h"

using namespace winrt::MainApplication;

namespace winrt::Helper
{
    template<typename ContentTy>
    inline Microsoft::UI::Xaml::Controls::ContentDialog CreateContentDialog(const Microsoft::UI::Xaml::XamlRoot& root, const hstring& title, const ContentTy& content, const bool add_confirm = true, const bool& can_close = true)
    {
        Controls::ContentDialog dialog;
        dialog.XamlRoot(root);
        dialog.Title(box_value(title));
        dialog.Content(box_value(content));

        if (add_confirm)
        {
			dialog.PrimaryButtonText(L"Confirm");
            dialog.DefaultButton(Microsoft::UI::Xaml::Controls::ContentDialogButton::Primary);
		}

        if (can_close)
        {
            dialog.CloseButtonText(L"Close");
        }

        return dialog;
    }

    inline Microsoft::UI::Xaml::Controls::ContentDialog CreateLoadingDialog(const Microsoft::UI::Xaml::XamlRoot& root)
    {
        Microsoft::UI::Xaml::Controls::ProgressBar progress_bar;
        progress_bar.IsIndeterminate(true);
        return Helper::CreateContentDialog(root, L"Loading...", progress_bar, false, false);
    }

    inline Windows::Foundation::IAsyncAction InvokeGeneratorDialog(const Microsoft::UI::Xaml::XamlRoot& root)
    {
        try
        {
            auto generator_content = winrt::make<MainApplication::implementation::PasswordGenerator>();
            auto generator_dialog = Helper::CreateContentDialog(root, L"Password Generator", generator_content, false, false);

            generator_content.OnClose([generator_dialog]()
            {
				generator_dialog.Hide();
			});

            co_await generator_dialog.ShowAsync();
        }
        catch (...)
        {
        }
    }

    inline Windows::Foundation::IAsyncAction InvokeSettingsDialog(const Microsoft::UI::Xaml::XamlRoot& root)
    {
        try
        {
            auto settings = winrt::make<MainApplication::implementation::ApplicationSettings>();
            settings.XamlRoot(root);
            co_await settings.ShowAsync();
        }
        catch (...)
        {
        }
    }
}