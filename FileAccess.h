///  FileAccess.h
/// <summary>
/// Header file for the FileAccess class
/// </summary>
/// <author> Brayden Folk </author>

#pragma once
#include "pch.h"
#include "MainPage.xaml.h"

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
		Windows::Storage::StorageFile^ file;
		void CreateFile();
		Windows::Storage::StorageFile^ _sampleFile;

	internal:
		property Windows::Storage::StorageFile^ SampleFile
		{
			Windows::Storage::StorageFile^ get()
			{
				return _sampleFile;
			}
			void set(Windows::Storage::StorageFile^ value)
			{
				_sampleFile = value;
			}
		}
	};
}