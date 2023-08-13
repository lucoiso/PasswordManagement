// Author: Lucas Oliveira Vilas-Bôas
// Year: 2023
// Repository: https://github.com/lucoiso/PasswordManagement

#pragma once

#include "Generator.g.h"

namespace winrt::MainApplication::implementation
{
    struct Generator : GeneratorT<Generator>
    {
        Generator() = default;

        static Windows::Foundation::IAsyncOperation<hstring> GeneratePassword();
    };
}

namespace winrt::MainApplication::factory_implementation
{
    struct Generator : GeneratorT<Generator, implementation::Generator>
    {
    };
}
