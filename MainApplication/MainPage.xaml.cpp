// Author: Lucas Oliveira Vilas-B�as
// Year: 2022
// Repository: https://github.com/lucoiso/PasswordManagement

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

    Windows::Foundation::IAsyncAction MainPage::performDataImport()
    {
        for (const auto& iterator : co_await Helper::loadPasswordDataFiles())
        {
            if (!iterator)
            {
                continue;
            }

            m_manager.importData(iterator);
        }

        saveLocalData();
    }

    Windows::Foundation::IAsyncAction MainPage::performDataExport()
    {
        if (const auto exported_file = co_await Helper::savePasswordDataFile(MO_DataOptions().SelectedExportDataType()))
        {
            m_manager.exportData(exported_file, LI_LoginData().Data().GetView());
        }
    }

    void MainPage::bindDataUpdate()
    {
        const auto update_data_lambda = [this]([[maybe_unused]] const auto&, PasswordManager::LoginUpdateEventParams const& event_data)
        {
            LI_LoginData().insertDataInList(event_data.Data());
        };

        m_data_update_token = m_manager.LoginDataUpdated(update_data_lambda);
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

    Windows::Foundation::IAsyncAction MainPage::loadLocalData()
    {
        try
        {
            m_manager.importData(co_await Helper::getLocalDataFile());
        }
        catch (const hresult_error& e)
        {
            Helper::printDebugLine(e.message());
        }

        co_return;
    }

    Windows::Foundation::IAsyncAction MainPage::saveLocalData()
    {
        try
        {
            m_manager.exportData(co_await Helper::getLocalDataFile(), LI_LoginData().Data().GetView());
        }
        catch (const hresult_error& e)
        {
            Helper::printDebugLine(e.message());
        }

        co_return;
    }
}
