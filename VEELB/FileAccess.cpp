#include "pch.h"
#include "FileAccess.h"

using namespace VEELB;

using namespace concurrency;
using namespace Platform;
using namespace Windows::Storage;
using namespace Windows::UI::Xaml;

FileAccess::FileAccess(Platform::String^ fileName1)
{
	fileName = fileName1;
	CreateFile();
}

void FileAccess::CreateFile()
{
	//null pointer allows for access to the current user

	//KnownFolderId::

	create_task(KnownFolders::GetFolderForUserAsync(nullptr, KnownFolderId::DocumentsLibrary))
		.then([this](StorageFolder^ documentsFolder)
	{
		Platform::String^ fileName1 = fileName;
		return documentsFolder->CreateFileAsync(fileName, CreationCollisionOption::OpenIfExists);
	}).then([this](task<StorageFile^> task)
	{
		try
		{
			sampleFile = task.get();
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
	StorageFile^ tempFile = sampleFile;
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
	StorageFile^ tempFile = sampleFile;
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