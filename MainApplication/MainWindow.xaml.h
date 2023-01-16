// Author: Lucas Oliveira Vilas-B�as
// Year: 2022
// Repository: https://github.com/lucoiso/PasswordManagement

#pragma once

#include "MainWindow.g.h"

namespace winrt::MainApplication::implementation
{
    struct MainWindow : MainWindowT<MainWindow>
    {
        MainWindow();
    };
}

namespace winrt::MainApplication::factory_implementation
{
    struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow>
    {
    };
}
