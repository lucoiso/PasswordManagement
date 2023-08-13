// Author: Lucas Oliveira Vilas-Bôas
// Year: 2023
// Repository: https://github.com/lucoiso/PasswordManagement

#include "pch.h"

#include "MainPage.xaml.h"
#include "MainPage.g.cpp"

#include "App.xaml.h"

#include "DataManager.h"

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MainApplication::implementation
{
    MainPage::MainPage()
    {
        InitializeComponent();
    }

    Windows::Foundation::IAsyncAction MainPage::ComponentLoaded([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] Microsoft::UI::Xaml::RoutedEventArgs const& args)
    {
        const auto main_window = Application::Current().as<App>()->Window();
        const auto ensure_save_lambda = [this, main_window]([[maybe_unused]] const auto& sender, [[maybe_unused]] const auto& args) -> Windows::Foundation::IAsyncAction
            {
                co_await DataManager::GetInstance().SaveLocalDataFileAsync();
            };

        main_window.Closed(ensure_save_lambda);
        co_await DataManager::GetInstance().Activate();
    }
}