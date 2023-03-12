// Author: Lucas Oliveira Vilas-Bôas
// Year: 2023
// Repository: https://github.com/lucoiso/PasswordManagement

#include "pch.h"

#include "DialogManager.h"

#include "App.xaml.h"

#include "ApplicationSettings.xaml.h"
#include "PasswordGenerator.xaml.h"

Microsoft::UI::Xaml::XamlRoot DialogManager::GetMainWindowContentXamlRoot() const
{
    return Microsoft::UI::Xaml::Application::Current().as<MainApplication::implementation::App>()->Window().as<MainApplication::MainWindow>().Content().XamlRoot();
}

bool DialogManager::HasAnyDialogOpen() const
{
    const auto existing_popups = Microsoft::UI::Xaml::Media::VisualTreeHelper::GetOpenPopupsForXamlRoot(GetMainWindowContentXamlRoot());
    for (const auto& popup : existing_popups)
    {
        if (const auto existing_dialog = popup.Child().try_as<Microsoft::UI::Xaml::Controls::ContentDialog>())
        {
            return true;
        }
    }

    return false;
}

void DialogManager::CloseExistingDialogs() const
{
    LUPASS_LOG_FUNCTION();

    const auto existing_popups = Microsoft::UI::Xaml::Media::VisualTreeHelper::GetOpenPopupsForXamlRoot(GetMainWindowContentXamlRoot());
    for (const auto& popup : existing_popups)
    {
        const auto existing_dialog = popup.Child().try_as<Microsoft::UI::Xaml::Controls::ContentDialog>();
        if (existing_dialog && existing_dialog != m_loading_dialog)
        {
            existing_dialog.Hide();
        }
    }
}

void DialogManager::ShowLoadingDialog()
{
    LUPASS_LOG_FUNCTION();

    if (!GetMainWindowContentXamlRoot())
    {
        return;
    }

    const bool can_show_loading = m_loading_dialog_count <= 0u;

    ++m_loading_dialog_count;

    CloseExistingDialogs();

    // Loading is already open
    if (HasAnyDialogOpen())
    {
        return;
    }

    if (can_show_loading)
    {
        Microsoft::UI::Xaml::Controls::ProgressBar progress_bar;
        progress_bar.IsIndeterminate(true);
        m_loading_dialog = CreateContentDialog(L"Loading...", progress_bar, false, false);

        m_loading_dialog.ShowAsync();
    }
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

Windows::Foundation::IAsyncAction DialogManager::InvokeSettingsDialogAsync()
{
    LUPASS_LOG_FUNCTION();

    if (HasAnyDialogOpen())
    {
        co_return;
    }

    try
    {
        auto settings = winrt::make<MainApplication::implementation::ApplicationSettings>();
        settings.XamlRoot(GetMainWindowContentXamlRoot());
        co_await settings.ShowAsync();
    }
    catch (const hresult_error& e)
    {
        Helper::PrintDebugLine(e.message());
    }
}

Windows::Foundation::IAsyncAction DialogManager::InvokeGeneratorDialogAsync()
{
    LUPASS_LOG_FUNCTION();

    if (HasAnyDialogOpen())
    {
        co_return;
    }

    try
    {
        auto generator_content = winrt::make<MainApplication::implementation::PasswordGenerator>();
        auto generator_dialog = CreateContentDialog(L"Password Generator", generator_content, false, false);

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
