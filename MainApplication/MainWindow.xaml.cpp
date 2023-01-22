// Author: Lucas Oliveira Vilas-Bôas
// Year: 2022
// Repository: https://github.com/lucoiso/PasswordManagement

#include "pch.h"

#include "MainWindow.xaml.h"
#include "MainWindow.g.cpp"

#include "MainPage.xaml.h"

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MainApplication::implementation
{
    MainWindow::MainWindow()
    {
        InitializeComponent();
        Title(L"Password Manager");
    }

    void MainApplication::implementation::MainWindow::FR_MainFrame_Loaded([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] RoutedEventArgs const& args)
    {
        InitializeContentAsync();
    }

    template<typename ContentTy>
	Controls::ContentDialog CreateContentDialog(const XamlRoot& root, const bool& can_close, const hstring& Title, const ContentTy& content)
	{
		Controls::ContentDialog dialog;
        dialog.XamlRoot(root);
        dialog.Title(box_value(Title));
		dialog.Content(box_value(content));
        
        if (can_close)
        {
            dialog.CloseButtonText(L"Close");
        }
        
		return dialog;
	}

    Windows::Foundation::IAsyncAction MainWindow::InitializeContentAsync()
    {
        Controls::ProgressBar progress_bar;
        progress_bar.IsIndeterminate(true);

		const auto loading_dialog = CreateContentDialog(Content().XamlRoot(), false, L"Loading...", progress_bar);
        loading_dialog.ShowAsync();

        const auto emit_error = [this, &loading_dialog] () -> Windows::Foundation::IAsyncAction {
            loading_dialog.Hide();
            
            const Controls::ContentDialog error_dialog = CreateContentDialog(Content().XamlRoot(), true, L"An error has occurred", L"Your device does not support the required features to run this application. Please enable Windows Hello! in your device settings.");
            co_await error_dialog.ShowAsync();
            Application::Current().Exit();
        };
        
        if (!(co_await Windows::Security::Credentials::KeyCredentialManager::IsSupportedAsync()))
        {
            co_await emit_error();
            co_return;
        }

        const auto username = unbox_value<hstring>(co_await Windows::System::User::GetDefault().GetPropertyAsync(Windows::System::KnownUserProperties::AccountName()));
        
        if (username.empty())
        {
            co_await emit_error();
            co_return;
        }
            
		switch ((co_await Windows::Security::Credentials::KeyCredentialManager::RequestCreateAsync(username, Windows::Security::Credentials::KeyCredentialCreationOption::ReplaceExisting)).Status())
        {
		    case Windows::Security::Credentials::KeyCredentialStatus::Success:
				FR_MainFrame().Navigate(xaml_typename<MainApplication::MainPage>());
			    break;
                
            default:
                Application::Current().Exit();
        }

        loading_dialog.Hide();
    }
}
