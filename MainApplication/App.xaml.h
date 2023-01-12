// Author: Lucas Oliveira Vilas-Bôas
// Year: 2022
// Repository:

#pragma once

#include "App.xaml.g.h"

namespace winrt::MainApplication::implementation
{
    struct App : AppT<App>
    {
        App();

        void OnLaunched(Microsoft::UI::Xaml::LaunchActivatedEventArgs const&);

    private:
        winrt::Microsoft::UI::Xaml::Window window{ nullptr };
    };
}
