// Author: Lucas Oliveira Vilas-Bôas
// Year: 2023
// Repository: https://github.com/lucoiso/PasswordManagement

#pragma once

#include "pch.h"

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
}