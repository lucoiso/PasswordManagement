// Author: Lucas Oliveira Vilas-B�as
// Year: 2022
// Repository: https://github.com/lucoiso/PasswordManagement

#include "pch.h"

#include "App.xaml.h"
#include "MainWindow.xaml.h"

#include <Microsoft.UI.Xaml.Window.h>

using namespace winrt;
using namespace Windows::Foundation;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Controls;
using namespace Microsoft::UI::Xaml::Navigation;
using namespace MainApplication;
using namespace MainApplication::implementation;

App::App()
{
    InitializeComponent();

#if defined _DEBUG && !defined DISABLE_XAML_GENERATED_BREAK_ON_UNHANDLED_EXCEPTION
    UnhandledException([this](IInspectable const&, UnhandledExceptionEventArgs const& e)
    {
        if (IsDebuggerPresent())
        {
            auto errorMessage = e.Message();
            __debugbreak();
        }
    });
#endif
}

void App::OnLaunched(LaunchActivatedEventArgs const&)
{
    m_window = make<MainWindow>();
    m_window.Activate();
}

Window MainApplication::implementation::App::Window() const
{
    return m_window;
}

HWND MainApplication::implementation::App::getWindowHandle()
{    
    const Microsoft::UI::Xaml::Window currentWindow = Application::Current().try_as<App>()->Window();
    check_bool(currentWindow);

    auto windowNative{ currentWindow.try_as<IWindowNative>() };
    check_bool(windowNative);

    HWND output{ 0 };
    windowNative->get_WindowHandle(&output);

    return output;
}
