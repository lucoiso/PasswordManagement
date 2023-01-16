// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#include "pch.h"
#include "MainPage.xaml.h"
#include "MainPage.g.cpp"

#include "FileLoadingHelper.h"

#include <Helper.h>

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MainApplication::implementation
{    
    MainPage::MainPage()
    {
        InitializeComponent();
        InitializeSubobjects();
    }

    void MainPage::InitializeSubobjects()
    {
        { // Event bindings
            bindDataUpdate();
            bindDataLoad();
        }
        
        loadLocalData();
    }

    winrt::Windows::Foundation::IAsyncAction MainPage::performDataImport()
    {
        for (const auto& iterator : co_await Helper::chooseFileInput())
        {
            if (!iterator)
            {
                continue;
            }

            if (iterator.FileType() == L".txt")
            {
                this->m_manager.importData(iterator);
            }
            else if (iterator.FileType() == L".csv")
            {
                this->m_manager.importData(iterator);
            }
        }

        saveLocalData();
    }

    winrt::Windows::Foundation::IAsyncAction MainPage::performDataExport()
    {
        if (const auto exported_file = co_await Helper::chooseFileDestination(MO_DataOptions().SelectedExportDataType()))
        {
            this->m_manager.exportData(exported_file, LI_LoginData().Data().GetView());
        }
    }

    void MainPage::bindDataUpdate()
    {
        const auto update_data_lambda = [this]([[maybe_unused]] const auto&, PasswordManager::LoginUpdateEventParams const& event_data)
        {
            LI_LoginData().insertDataInList(event_data.Data());
        };

        this->m_data_update_token = this->m_manager.LoginDataUpdated(update_data_lambda);
    }

    void MainPage::bindDataLoad()
    {
        const auto import_data_lambda = [this]()
        {
            try
            {
                performDataImport();
            }
            catch (const hresult_error& e)
            {
                Helper::printDebugLine(e.message());
            }
        };

		m_data_import_token = MO_DataOptions().ImportPressed(import_data_lambda);

        const auto export_data_lambda = [this]()
        {
            try
            {
                performDataExport();
            }
            catch (const hresult_error& e)
            {
                Helper::printDebugLine(e.message());
            }
        };

        m_data_export_token = MO_DataOptions().ExportPressed(export_data_lambda);
    }

    winrt::Windows::Foundation::IAsyncAction MainPage::loadLocalData()
    {
        try
        {
            this->m_manager.importData(co_await Helper::getLocalDataFile());
        }
        catch (const hresult_error& e)
        {
            Helper::printDebugLine(e.message());
        }

        co_return;
    }

    winrt::Windows::Foundation::IAsyncAction MainPage::saveLocalData()
    {
        try
        {
            this->m_manager.exportData(co_await Helper::getLocalDataFile(), LI_LoginData().Data().GetView());
        }
        catch (const hresult_error& e)
        {
            Helper::printDebugLine(e.message());
        }

        co_return;
    }
}
