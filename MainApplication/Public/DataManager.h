// Author: Lucas Oliveira Vilas-Bôas
// Year: 2023
// Repository: https://github.com/lucoiso/PasswordManagement

#pragma once

#include "pch.h"

class DataManager
{
private:
	DataManager() = default;

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

	Windows::Foundation::Collections::IVectorView<PasswordManager::LoginData> Data(const bool apply_filter = false) const;
	uint32_t DataSize() const;

	Windows::Foundation::Collections::IVectorView<uint64_t> Backups() const;

	Windows::Foundation::IAsyncAction InsertLoginDataAsync(const std::vector<PasswordManager::LoginData>& data, const bool save_data);
	Windows::Foundation::IAsyncAction RemoveLoginDataAsync(const std::vector<PasswordManager::LoginData>& data, const bool save_data);

	Windows::Foundation::IAsyncAction ClearLoginDataAsync(const bool save_data);

	Windows::Foundation::IAsyncAction ImportDataFromFileAsync(const std::vector<Windows::Storage::StorageFile>& data, const bool save_data);
	Windows::Foundation::IAsyncAction ReplaceDataWithFileAsync(const std::vector<Windows::Storage::StorageFile>& data, const bool save_data);

	Windows::Foundation::IAsyncAction ExportDataAsync(const PasswordManager::LoginDataExportType export_type) const;

	Windows::Foundation::IAsyncOperation<Windows::Foundation::Collections::IVectorView<Windows::Storage::StorageFile>> LoadPasswordDataFilesAsync() const;
	Windows::Foundation::IAsyncOperation<Windows::Storage::StorageFile> SavePasswordDataFileAsync(const PasswordManager::LoginDataExportType& export_mode) const;

	Windows::Foundation::IAsyncOperation<Windows::Storage::StorageFolder> GetBackupsDirectoryAsync() const;
	Windows::Foundation::IAsyncOperation<Windows::Storage::StorageFile> GetLocalDataFileAsync() const;

	Windows::Foundation::IAsyncAction SaveLocalDataFileAsync() const;

	Windows::Foundation::IAsyncAction CopyCurrentDataAsBackupAsync() const;
	Windows::Foundation::IAsyncAction RefreshBackupDataAsync();

	event_token DataSync(MainApplication::SingleVoidDelegate const& handler);
	void DataSync(event_token const& token) noexcept;

private:
	bool m_activated{ false };
	PasswordManager::LoginDataManager m_manager;

	hstring m_search_text;
	MainApplication::DataSortMode m_sorting_mode;
	MainApplication::DataSortOrientation m_sorting_orientation;

	std::vector<PasswordManager::LoginData> m_data;
	std::vector<uint64_t> m_backups;

	event_token m_data_update_token;
	event<MainApplication::SingleVoidDelegate> m_data_synchronized_event;
};
