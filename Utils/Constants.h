// Author: Lucas Oliveira Vilas-Bôas
// Year: 2023
// Repository: https://github.com/lucoiso/PasswordManagement

#pragma once

#ifndef LUPASS_CONSTANTS_H
#define LUPASS_CONSTANTS_H

#ifndef _DEBUG
#define _DEBUG 0
#endif

constexpr bool ENABLE_DEBBUGGING = _DEBUG;
constexpr bool ENABLE_RELEASE_TESTING = true;

constexpr wchar_t APP_INSTANCE_KEY[] = L"LUCOISO_PASS_MANAGER_INSTANCE_KEY";
constexpr wchar_t APP_PRODUCT_ID[] = L"9NBM0FRN8VS7";
constexpr wchar_t APP_PRODUCT_URI[] = L"ms-windows-store://pdp/?ProductId=9NBM0FRN8VS7";
constexpr wchar_t APP_SUBSCRIPTION_PRODUCT_ID[] = L"9N6CXRRVCV2R";
constexpr wchar_t APP_SUBSCRIPTION_URI[] = L"ms-windows-store://pdp/?ProductId=9N6CXRRVCV2R";
constexpr wchar_t APP_NAME[] = L"Lupass";
constexpr wchar_t ICON_NAME[] = L"Icon.ico";
constexpr wchar_t APP_VERSION[] = L"v1.0.2";
constexpr wchar_t TRAYICON_CLASSNAME[] = L"LUCOISO_PASS_MANAGER_TRAY_ICON_CN";

constexpr wchar_t APP_DATA_FILE_NAME[] = L"APPLICATION_DATA";
constexpr wchar_t APP_BACKUP_DATA_DIRECTORY_NAME[] = L"Backups";

constexpr wchar_t PASSWORD_DATA_LUPASS_HEADER[] = L"name,url,username,password,notes,created,changed,used";
constexpr wchar_t PASSWORD_DATA_MICROSOFT_HEADER[] = L"name,url,username,password";
constexpr wchar_t PASSWORD_DATA_GOOGLE_HEADER[] = L"name,url,username,password,notes";
constexpr wchar_t PASSWORD_DATA_FIREFOX_HEADER[] = L"\"url\",\"username\",\"password\",\"httpRealm\",\"formActionOrigin\",\"guid\",\"timeCreated\",\"timeLastUsed\",\"timePasswordChanged\"";
constexpr wchar_t PASSWORD_DATA_KAPERSKY_HEADER[] = L"Websites";

constexpr wchar_t KEY_MATERIAL_TEMP_ID[] = L"KEY_MATERIAL_TEMP_ID";

constexpr wchar_t SECURITY_KEY_SET_ID[] = L"SECURITY_KEY_SET_ID";
constexpr wchar_t INVALID_SECURITY_ENVIRONMENT_ID[] = L"INVALID_SECURITY_ENVIRONMENT_ID";

constexpr wchar_t SETTING_ENABLE_WINDOWS_HELLO[] = L"SETTING_ENABLE_WINDOWS_HELLO";
constexpr wchar_t SETTING_ENABLE_SYSTEM_TRAY[] = L"SETTING_ENABLE_SYSTEM_TRAY";
constexpr wchar_t SETTING_ENABLE_SHORTCUTS[] = L"SETTING_ENABLE_SHORTCUTS";

constexpr wchar_t SETTING_GENERATOR_LAST_GENERATED_PASSWORD[] = L"SETTING_GENERATOR_LAST_GENERATED_PASSWORD";
constexpr wchar_t SETTING_GENERATOR_ENABLE_LOWERCASE[] = L"SETTING_GENERATOR_ENABLE_LOWERCASE";
constexpr wchar_t SETTING_GENERATOR_ENABLE_UPPERCASE[] = L"SETTING_GENERATOR_ENABLE_UPPERCASE";
constexpr wchar_t SETTING_GENERATOR_ENABLE_NUMBERS[] = L"SETTING_GENERATOR_ENABLE_NUMBERS";
constexpr wchar_t SETTING_GENERATOR_ENABLE_CUSTOM_CHARACTERS[] = L"SETTING_GENERATOR_ENABLE_CUSTOM_CHARACTERS";
constexpr wchar_t SETTING_GENERATOR_CUSTOM_CHARACTERS[] = L"SETTING_GENERATOR_CUSTOM_CHARACTERS";
constexpr wchar_t SETTING_GENERATOR_PASSWORD_SIZE[] = L"SETTING_GENERATOR_PASSWORD_SIZE";

#endif
