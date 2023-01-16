// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#pragma once

#include "winrt/Microsoft.UI.Xaml.h"
#include "winrt/Microsoft.UI.Xaml.Markup.h"
#include "winrt/Microsoft.UI.Xaml.Controls.Primitives.h"
#include "LoginDataListMenu.g.h"

namespace winrt::MainApplication::implementation
{
    struct LoginDataListMenu : LoginDataListMenuT<LoginDataListMenu>
    {
        LoginDataListMenu();

        void BP_Import_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& args);
        void BP_Export_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& args);

        winrt::PasswordManager::LoginDataFileType SelectedExportDataType();
        DataSortMode SelectedSortingMode();
        DataSortOrientation SelectedSortingOrientation();
        
        winrt::event_token ImportPressed(MainApplication::SingleVoidDelegate const& handler);
        void ImportPressed(winrt::event_token const& token) noexcept;

        winrt::event_token ExportPressed(MainApplication::SingleVoidDelegate const& handler);
        void ExportPressed(winrt::event_token const& token) noexcept;

    private:
		winrt::event<MainApplication::SingleVoidDelegate> m_import_pressed;
		winrt::event<MainApplication::SingleVoidDelegate> m_export_pressed;
    };
}

namespace winrt::MainApplication::factory_implementation
{
    struct LoginDataListMenu : LoginDataListMenuT<LoginDataListMenu, implementation::LoginDataListMenu>
    {
    };
}
