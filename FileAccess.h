#pragma once
#include "pch.h"
#include "MainPage.xaml.h"

namespace VEELB
{
	public ref class FileAccess sealed
	{
	public:
		FileAccess(Platform::String^ fileName);
		bool WriteTextToFile(Platform::String^ inputText);
		Platform::String^ ReadTextFromFile();
	private:
		Platform::String^ fileName;
		Windows::Storage::StorageFile^ file;
		void CreateFile();
		Windows::Storage::StorageFile^ sampleFile;
	internal:
		property Windows::Storage::StorageFile^ SampleFile
		{
			Windows::Storage::StorageFile^ get()
			{
				return sampleFile;
			}
			void set(Windows::Storage::StorageFile^ value)
			{
				sampleFile = value;
			}
		}
	};
}