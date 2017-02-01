#include "pch.h"
#include "FileAccess.h"

using namespace VEELB;

using namespace concurrency;
using namespace Platform;
using namespace Windows::Storage;
using namespace Windows::UI::Xaml;

//FileAccess::FileAccess(Platform::String^ fileName1)
//{
//	fileName = fileName1;
//}
//
//void FileAccess::CreateFile()
//{
//	//null pointer allows for access to the current user
//
//	create_task(KnownFolders::GetFolderForUserAsync(nullptr, KnownFolderId::PicturesLibrary))
//		.then([this](StorageFolder^ picturesFolder)
//	{
//		Platform::String^ fileName1 = fileName;
//		return picturesFolder->CreateFileAsync(fileName, CreationCollisionOption::ReplaceExisting);
//	}).then([this](task<StorageFile^> task)
//	{
//		try
//		{
//			file = task.get();
//			// success
//		}
//		catch (Platform::Exception^ e)
//		{
//			// I/O errors are reported as exceptions.
//			// TODO: notify error
//		}
//	});
//}
//
//bool FileAccess::WriteTextToFile(Platform::String^ inputText)
//{
//	StorageFile^ tempFile = file;
//	if (tempFile != nullptr)
//	{
//		Platform::String^ userContent = inputText;
//		if (userContent != nullptr && !userContent->IsEmpty())
//		{
//			create_task(FileIO::WriteTextAsync(tempFile, userContent)).then([this, tempFile, userContent](task<void> task)
//			{
//				try
//				{
//					task.get();
//				}
//				catch (COMException^ ex)
//				{
//					// TODO: notify error
//				}
//			});
//		}
//		else
//		{
//		}
//	}
//	else
//	{
//		//// TODO: notify file does not exist error
//	}
//}
//
//Platform::String^ FileAccess::ReadTextFromFile()
//{
//	StorageFile^ tempFile = file;
//	if (tempFile != nullptr)
//	{
//		create_task(FileIO::ReadTextAsync(tempFile)).then([this, tempFile](task<Platform::String^> task)
//		{
//			try
//			{
//				Platform::String^ fileContent = task.get();
//			}
//			catch (COMException^ ex)
//			{
//				// TODO: notify error
//			}
//		});
//	}
//	else
//	{
//		// TODO: notify file does not exist error
//	}
//}