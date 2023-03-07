// Author: Lucas Oliveira Vilas-Bôas
// Year: 2023
// Repository: https://github.com/lucoiso/PasswordManagement

#pragma once

#ifndef LUPASS_DIALOG_HELPER_H
#define LUPASS_DIALOG_HELPER_H

#include "pch.h"

#include "App.xaml.h"

#include "ApplicationSettings.xaml.h"
#include "PasswordGenerator.xaml.h"

using namespace winrt::MainApplication;

namespace winrt::Helper
{
    template<typename ContentTy>
    inline Microsoft::UI::Xaml::Controls::ContentDialog CreateContentDialog(const Microsoft::UI::Xaml::XamlRoot& root, const hstring& title, const ContentTy& content, const bool add_confirm = true, const bool& can_close = true, const hstring& confirm_text = L"Confirm", const hstring& close_text = L"Close")
    {
        LUPASS_LOG_FUNCTION();

        Controls::ContentDialog dialog;        

        dialog.XamlRoot(root);
        dialog.Title(box_value(title));
        dialog.Content(box_value(content));

        if (add_confirm)
        {
            dialog.PrimaryButtonText(confirm_text);
            dialog.DefaultButton(Microsoft::UI::Xaml::Controls::ContentDialogButton::Primary);
        }

        if (can_close)
        {
            dialog.CloseButtonText(close_text);
        }

        return dialog;
    }

    inline bool HasAnyPopupOpen()
    {
        return Microsoft::UI::Xaml::Media::VisualTreeHelper::GetOpenPopups(Microsoft::UI::Xaml::Application::Current().as<winrt::MainApplication::implementation::App>()->Window()).Size() > 0;
    }

    inline void CloseExistingDialogs()
    {
        LUPASS_LOG_FUNCTION();

        const auto existing_popups = Microsoft::UI::Xaml::Media::VisualTreeHelper::GetOpenPopups(Microsoft::UI::Xaml::Application::Current().as<winrt::MainApplication::implementation::App>()->Window());
        for (const auto& popup : existing_popups)
        {
            auto existing_dialog = popup.Child().try_as<Microsoft::UI::Xaml::Controls::ContentDialog>();
            if (existing_dialog)
            {
                existing_dialog.Hide();
            }
        }
    }

    inline Microsoft::UI::Xaml::Controls::ContentDialog CreateLoadingDialog(const Microsoft::UI::Xaml::XamlRoot& root)
    {
        LUPASS_LOG_FUNCTION();

        Microsoft::UI::Xaml::Controls::ProgressBar progress_bar;
        progress_bar.IsIndeterminate(true);
        return Helper::CreateContentDialog(root, L"Loading...", progress_bar, false, false);
    }

    inline Windows::Foundation::IAsyncAction InvokeGeneratorDialog(const Microsoft::UI::Xaml::XamlRoot& root)
    {
        LUPASS_LOG_FUNCTION();

        if (Microsoft::UI::Xaml::Media::VisualTreeHelper::GetOpenPopupsForXamlRoot(root).Size() > 0)
        {
            co_return;
        }

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
        catch (const hresult_error& e)
        {
            Helper::PrintDebugLine(e.message());
        }
    }

    inline Windows::Foundation::IAsyncAction InvokeSettingsDialog(const Microsoft::UI::Xaml::XamlRoot& root)
    {
        LUPASS_LOG_FUNCTION();

        if (Microsoft::UI::Xaml::Media::VisualTreeHelper::GetOpenPopupsForXamlRoot(root).Size() > 0)
        {
            co_return;
        }

        try
        {
            auto settings = winrt::make<MainApplication::implementation::ApplicationSettings>();
            settings.XamlRoot(root);
            co_await settings.ShowAsync();
        }
        catch (const hresult_error& e)
        {
            Helper::PrintDebugLine(e.message());
        }
    }
}
#endif
