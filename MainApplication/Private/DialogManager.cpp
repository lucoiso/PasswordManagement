// Author: Lucas Oliveira Vilas-Bôas
// Year: 2023
// Repository: https://github.com/lucoiso/PasswordManagement

#include "pch.h"

#include "DialogManager.h"

#include "ApplicationSettings.xaml.h"
#include "PasswordGenerator.xaml.h"

#include <thread>
#include <future>

void DialogManager::WaitForDialogClose(const Microsoft::UI::Xaml::XamlRoot& root) const
{
    std::unique_lock<std::mutex> lock(mutex);
    condition_variable.wait(lock, [this, &root] { return !HasAnyDialogOpen(root); });
}

void DialogManager::NotifyDialogClose(const Microsoft::UI::Xaml::XamlRoot& root) const
{
    if (!HasAnyDialogOpen(root))
    {
        condition_variable.notify_all();
    }
}

bool DialogManager::HasAnyDialogOpen(const Microsoft::UI::Xaml::XamlRoot& root) const
{
    const auto existing_popups = Microsoft::UI::Xaml::Media::VisualTreeHelper::GetOpenPopupsForXamlRoot(root);
    for (const auto& popup : existing_popups)
    {
        if (const auto existing_dialog = popup.Child().try_as<Microsoft::UI::Xaml::Controls::ContentDialog>())
        {
            return true;
        }        
    }

    return false;
}

void DialogManager::CloseExistingDialogs(const Microsoft::UI::Xaml::XamlRoot& root) const
{
    LUPASS_LOG_FUNCTION();

    const auto existing_popups = Microsoft::UI::Xaml::Media::VisualTreeHelper::GetOpenPopupsForXamlRoot(root);
    for (const auto& popup : existing_popups)
    {
        const auto existing_dialog = popup.Child().try_as<Microsoft::UI::Xaml::Controls::ContentDialog>();
        if (existing_dialog && existing_dialog != m_loading_dialog)
        {
            existing_dialog.Hide();
        }
    }
}

void DialogManager::ShowLoadingDialog(const Microsoft::UI::Xaml::XamlRoot& root)
{
    LUPASS_LOG_FUNCTION();

    if (!root)
    {
        return;
    }

    ++m_loading_dialog_count;

    CloseExistingDialogs(root);

    // Loading is already open
    if (HasAnyDialogOpen(root))
    {
        return;
    }

    Microsoft::UI::Xaml::Controls::ProgressBar progress_bar;
    progress_bar.IsIndeterminate(true);
    m_loading_dialog = CreateContentDialog(root, L"Loading...", progress_bar, false, false);

    m_loading_dialog.ShowAsync();
}

void DialogManager::HideLoadingDialog()
{
    LUPASS_LOG_FUNCTION();

    if (m_loading_dialog_count <= 0u)
    {
        m_loading_dialog_count = 0u;
    }
    else
    {
        --m_loading_dialog_count;
    }

    if (m_loading_dialog && m_loading_dialog_count <= 0u)
    {
        m_loading_dialog.Hide();
    }
}

Windows::Foundation::IAsyncAction DialogManager::InvokeSettingsDialogAsync(const Microsoft::UI::Xaml::XamlRoot& root)
{
    LUPASS_LOG_FUNCTION();

    if (HasAnyDialogOpen(root))
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

Windows::Foundation::IAsyncAction DialogManager::InvokeGeneratorDialogAsync(const Microsoft::UI::Xaml::XamlRoot& root)
{
    LUPASS_LOG_FUNCTION();

    if (HasAnyDialogOpen(root))
    {
        co_return;
    }

    try
    {
        auto generator_content = winrt::make<MainApplication::implementation::PasswordGenerator>();
        auto generator_dialog = CreateContentDialog(root, L"Password Generator", generator_content, false, false);

        generator_content.OnClose(
            [generator_dialog]()
            {
                generator_dialog.Hide();
            }
        );

        co_await generator_dialog.ShowAsync();
    }
    catch (const hresult_error& e)
    {
        Helper::PrintDebugLine(e.message());
    }
}
