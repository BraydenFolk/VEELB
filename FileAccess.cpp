/// FileAccess.cpp
/// <summary>
/// Implementation of FileAccess class. Provides methods for accessing the config file where settings are stored.
/// </summary>
/// <author> Brayden Folk </author>
/// <author> Petra Kujawa </author>

#include "pch.h"
#include "FileAccess.h"

using namespace VEELB;

using namespace concurrency;
using namespace Platform;
using namespace Windows::Storage;
using namespace Windows::UI::Xaml;

FileAccess::FileAccess(Platform::String^ _fileName1)
{
	_fileName = _fileName1;
	CreateFile();
}

void FileAccess::CreateFile()
{
	//null pointer allows for access to the current user

	//KnownFolderId::

	create_task(KnownFolders::GetFolderForUserAsync(nullptr, KnownFolderId::DocumentsLibrary))
		.then([this](StorageFolder^ documentsFolder)
	{
		Platform::String^ _fileName1 = _fileName;
		return documentsFolder->CreateFileAsync(_fileName, CreationCollisionOption::OpenIfExists);
	}).then([this](task<StorageFile^> task)
	{
		try
		{
			_sampleFile = task.get();
			// success
		}
		catch (Platform::Exception^ e)
		{
			// I/O errors are reported as exceptions.
			// TODO: notify error
		}
	});
}

bool FileAccess::WriteTextToFile(Platform::String^ inputText)
{
	StorageFile^ tempFile = _sampleFile;
	if (tempFile != nullptr)
	{
		Platform::String^ userContent = inputText;
		if (userContent != nullptr && !userContent->IsEmpty())
		{
			create_task(FileIO::WriteTextAsync(tempFile, userContent)).then([this, tempFile, userContent](task<void> task)
			{
				try
				{
					task.get();
					return true;
				}
				catch (COMException^ ex)
				{
					// TODO: notify error
					return false;
				}
			});
		}
		else
		{
			return false;
		}
	}
	else
	{
		//// TODO: notify file does not exist error
	}
}

Platform::String^ FileAccess::ReadTextFromFile()
{
	StorageFile^ tempFile = _sampleFile;
	if (tempFile != nullptr)
	{
		create_task(FileIO::ReadTextAsync(tempFile)).then([this, tempFile](task<Platform::String^> task)
		{
			try
			{
				Platform::String^ fileContent = task.get();
				return fileContent;
			}
			catch (COMException^ ex)
			{
				// TODO: notify error
				Platform::String^ fileError = "Error";
				return fileError;
			}
		});
	}
	else
	{
		// TODO: notify file does not exist error
		return "Error";
	}
}