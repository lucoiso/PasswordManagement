// Author: Lucas Oliveira Vilas-B�as
// Year: 2023
// Repository: https://github.com/lucoiso/PasswordManagement

#include "pch.h"

#include "LoginUpdateEventParams.h"
#include "LoginUpdateEventParams.g.cpp"

using namespace winrt;

namespace winrt::PasswordManager::implementation
{
    LoginUpdateEventParams::LoginUpdateEventParams(Windows::Foundation::Collections::IVector<PasswordManager::LoginData> const& data, PasswordManager::LoginDataFileType const& type) : m_data(data), m_type(type)
    {
    }

    Windows::Foundation::Collections::IVectorView<PasswordManager::LoginData> LoginUpdateEventParams::Data() const
    {
        LUPASS_LOG_FUNCTION();

        return m_data.GetView();
    }

    PasswordManager::LoginDataFileType LoginUpdateEventParams::Type() const
    {
        LUPASS_LOG_FUNCTION();

        return m_type;
    }
}