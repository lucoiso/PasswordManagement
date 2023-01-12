// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#pragma once

#include "Class.g.h"

namespace winrt::PasswordGenerator::implementation
{
    struct Class : ClassT<Class>
    {
        Class() = default;

        int32_t MyProperty();
        void MyProperty(int32_t value);
    };
}

namespace winrt::PasswordGenerator::factory_implementation
{
    struct Class : ClassT<Class, implementation::Class>
    {
    };
}
