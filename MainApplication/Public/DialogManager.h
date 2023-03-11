// Author: Lucas Oliveira Vilas-B�as
// Year: 2023
// Repository: https://github.com/lucoiso/PasswordManagement

#pragma once

#include "pch.h"

#include <mutex>
#include <condition_variable>

class DialogManager
{
private:
    DialogManager() = default;

    mutable std::mutex mutex;
    mutable std::condition_variable condition_variable;

    Microsoft::UI::Xaml::Controls::ContentDialog m_loading_dialog;

    unsigned int m_loading_dialog_count = 0u;

    template<typename ContentTy>
    inline Microsoft::UI::Xaml::Controls::ContentDialog CreateContentDialog(const hstring& title, const ContentTy& content, const bool add_confirm = true, const bool& can_close = true, const hstring& confirm_text = L"Confirm", const hstring& close_text = L"Close") const
    {
        LUPASS_LOG_FUNCTION();

        CloseExistingDialogs();

        // Loading is open
        if (HasAnyDialogOpen())
        {
            return nullptr;
        }

        Microsoft::UI::Xaml::Controls::ContentDialog dialog;
        dialog.XamlRoot(GetMainWindowContentXamlRoot());
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

public:
    static inline DialogManager& GetInstance()
    {
		static DialogManager instance;
		return instance;
	}

	DialogManager(const DialogManager&) = delete;
	DialogManager(DialogManager&&) = delete;
	DialogManager& operator=(const DialogManager&) = delete;
	DialogManager& operator=(DialogManager&&) = delete;

    Microsoft::UI::Xaml::XamlRoot GetMainWindowContentXamlRoot() const;

    void WaitForDialogClose() const;
    void NotifyDialogClose() const;
    bool HasAnyDialogOpen() const;
    void CloseExistingDialogs() const;

    template<typename ContentTy>
    inline Microsoft::UI::Xaml::Controls::ContentDialog ShowDialogSync(const hstring& title, const ContentTy content, const bool add_confirm = true, const bool& can_close = true, const hstring& confirm_text = L"Confirm", const hstring& close_text = L"Close") const
    {
        LUPASS_LOG_FUNCTION();

        const auto dialog = CreateContentDialog(title, content, add_confirm, can_close, confirm_text, close_text);
        if (dialog)
        {
            dialog.ShowAsync();
        }

        return dialog;
    }

    template<typename ContentTy>
    inline Windows::Foundation::IAsyncOperation<Microsoft::UI::Xaml::Controls::ContentDialogResult> ShowDialogAsync(const hstring& title, const ContentTy content, const bool add_confirm = true, const bool& can_close = true, const hstring& confirm_text = L"Confirm", const hstring& close_text = L"Close") const
    {
        LUPASS_LOG_FUNCTION();

        const auto dialog = CreateContentDialog(title, content, add_confirm, can_close, confirm_text, close_text);
        if (dialog)
        {
            co_return co_await dialog.ShowAsync();
        }

        co_return Microsoft::UI::Xaml::Controls::ContentDialogResult::None;
    }

    void ShowLoadingDialog();
    void HideLoadingDialog();

    Windows::Foundation::IAsyncAction InvokeSettingsDialogAsync();
    Windows::Foundation::IAsyncAction InvokeGeneratorDialogAsync();
};
