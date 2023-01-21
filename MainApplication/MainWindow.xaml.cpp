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
        Title(L"Password Management");
    }

    void MainApplication::implementation::MainWindow::FR_MainFrame_Loaded([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] RoutedEventArgs const& args)
    {
        InitializeContentAsync();
    }

    Windows::Foundation::IAsyncAction MainWindow::InitializeContentAsync()
    {
        const auto emit_error = [this] () -> Windows::Foundation::IAsyncAction {
            Controls::ContentDialog error_dialog;
            error_dialog.XamlRoot(Content().XamlRoot());
            error_dialog.Title(box_value(L"An error has occurred"));
            error_dialog.Content(box_value(L"Your device does not support the required features to run this application. Please enable Windows Hello! in your device settings."));
            error_dialog.CloseButtonText(L"Close");
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
                co_return;
        }
    }
}
