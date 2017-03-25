///  FileAccess.h
/// <summary>
/// Header file for the FileAccess class
/// </summary>
/// <author> Brayden Folk </author>


#pragma once
#include "pch.h"
#include "MainPage.xaml.h"

using namespace Windows::Storage;

namespace VEELB
{
	public ref class FileAccess sealed
	{
	public:
		FileAccess(Platform::String^ _fileName);
		bool WriteTextToFile(Platform::String^ inputText);
		Platform::String^ ReadTextFromFile();

	private:
		Platform::String^ _fileName;
		StorageFile^ _file;
		void CreateFile();
		StorageFile^ _sampleFile;

	internal:
		property StorageFile^ _SampleFile
		{
			StorageFile^ get()
			{
				return _sampleFile;
			}
			void set(StorageFile^ value)
			{
				_sampleFile = value;
			}
		}
	};
}