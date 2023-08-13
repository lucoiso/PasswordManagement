// Author: Lucas Oliveira Vilas-Bôas
// Year: 2023
// Repository: https://github.com/lucoiso/PasswordManagement

#pragma once

#include "pch.h"

class DataManager
{
private:
    DataManager();
    ~DataManager();

public:
    static inline DataManager& GetInstance()
    {
        static DataManager instance;
        return instance;
    }

    DataManager(const DataManager&) = delete;
    DataManager(DataManager&&) = delete;
    DataManager& operator=(const DataManager&) = delete;
    DataManager& operator=(DataManager&&) = delete;

    Windows::Foundation::IAsyncAction Activate();
    void NotifyDataChange();

    void SetSearchText(hstring const& value);
    void SetSortingMode(MainApplication::DataSortMode const& value);
    void SetSortingOrientation(MainApplication::DataSortOrientation const& value);

    Windows::Foundation::Collections::IVectorView<MainApplication::LoginData> Data(const bool apply_filter = false) const;
    uint32_t DataSize() const;

    Windows::Foundation::Collections::IVectorView<uint64_t> Backups() const;

    Windows::Foundation::IAsyncAction InsertLoginDataAsync(const std::vector<MainApplication::LoginData>& data, const bool save_data);
    Windows::Foundation::IAsyncAction RemoveLoginDataAsync(const std::vector<MainApplication::LoginData>& data, const bool save_data);

    Windows::Foundation::IAsyncAction ClearLoginDataAsync(const bool save_data);

    Windows::Foundation::IAsyncAction ImportDataFromFileAsync(const std::vector<Windows::Storage::StorageFile>& data, const bool save_data);
    Windows::Foundation::IAsyncAction ReplaceDataWithFileAsync(const std::vector<Windows::Storage::StorageFile>& data, const bool save_data);

    Windows::Foundation::IAsyncAction ExportDataAsync(const MainApplication::LoginDataFileType export_type) const;

    Windows::Foundation::IAsyncOperation<Windows::Foundation::Collections::IVectorView<Windows::Storage::StorageFile>> LoadPasswordDataFilesAsync() const;
    Windows::Foundation::IAsyncOperation<Windows::Storage::StorageFile> SavePasswordDataFileAsync(const MainApplication::LoginDataFileType& export_mode) const;

    Windows::Foundation::IAsyncOperation<Windows::Storage::StorageFolder> GetBackupsDirectoryAsync() const;
    Windows::Foundation::IAsyncOperation<Windows::Storage::StorageFile> GetLocalDataFileAsync() const;

    Windows::Foundation::IAsyncAction SaveLocalDataFileAsync() const;

    Windows::Foundation::IAsyncAction CopyCurrentDataAsBackupAsync() const;
    Windows::Foundation::IAsyncAction RefreshBackupDataAsync();

    event_token DataSync(MainApplication::SingleVoidDelegate const& handler);
    void DataSync(event_token const& token) noexcept;

private:

    Windows::Foundation::IAsyncAction ImportDataAsync(Windows::Storage::StorageFile const& file, const MainApplication::LoginDataFileType import_type);

    Windows::Foundation::IAsyncAction ExportDataAsync(Windows::Storage::StorageFile const& file, Windows::Foundation::Collections::IVectorView<MainApplication::LoginData> const& data, const MainApplication::LoginDataFileType export_type) const;

    Windows::Foundation::IAsyncAction PushData(const std::vector<MainApplication::LoginData>& data, const MainApplication::LoginDataFileType type);

    Windows::Foundation::IAsyncOperation<Windows::Foundation::Collections::IVector<MainApplication::LoginData>> ReadData_Lupass(hstring const& display_name, Windows::Storage::Streams::IBuffer const& data_buffer);
    Windows::Foundation::IAsyncAction WriteData_Lupass(Windows::Storage::StorageFile const& file, Windows::Foundation::Collections::IVectorView<MainApplication::LoginData> const& data) const;

    std::vector<MainApplication::LoginData> ReadData_GenericCSV(Windows::Foundation::Collections::IVectorView<hstring> const& file_text, const MainApplication::LoginDataFileType data_type);
    Windows::Foundation::IAsyncAction WriteData_GenericCSV(Windows::Storage::StorageFile const& file, Windows::Foundation::Collections::IVectorView<MainApplication::LoginData> const& data, const MainApplication::LoginDataFileType data_type) const;

    void ProcessCsvLine(hstring const& line, std::vector<MainApplication::LoginData>& output_data, const MainApplication::LoginDataFileType data_type) const;

    std::vector<MainApplication::LoginData> ReadData_Kapersky(Windows::Foundation::Collections::IVectorView<hstring> const& file_text);
    Windows::Foundation::IAsyncAction WriteData_Kapersky(Windows::Storage::StorageFile const& file, Windows::Foundation::Collections::IVectorView<MainApplication::LoginData> const& data) const;

private:
    bool m_activated{ false };

    hstring m_search_text;
    MainApplication::DataSortMode m_sorting_mode;
    MainApplication::DataSortOrientation m_sorting_orientation;

    std::vector<MainApplication::LoginData> m_data;
    std::vector<uint64_t> m_backups;

    event<MainApplication::SingleVoidDelegate> m_data_synchronized_event;
};
