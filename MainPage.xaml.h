//
// MainPage.xaml.h
// Header file for the main class
//

#pragma once

#include "MainPage.g.h"
#include "Device.h"
#include <opencv2\core\core.hpp>

using namespace VEELB;
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Media::Imaging;
using namespace Windows::Storage::Streams;
using namespace Microsoft::WRL;
using namespace cv;
using namespace std;


namespace VEELB
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	public ref class MainPage sealed
	{
	public: // Properties
		MainPage();
		// Serial comms
		static Windows::Foundation::IAsyncOperation<Windows::Devices::Enumeration::DeviceInformationCollection ^> ^ListAvailableSerialDevicesAsync(void);

		// For XAML binding purposes, use the IObservableVector interface containing Object^ objects. 
		// This wraps the real implementation of _availableDevices which is implemented as a Vector.
		// See "Data Binding Overview (XAML)" https://msdn.microsoft.com/en-us/library/windows/apps/xaml/hh758320.aspx
		property Windows::Foundation::Collections::IObservableVector<Platform::Object^>^ AvailableDevices
		{
			Windows::Foundation::Collections::IObservableVector<Platform::Object^>^ get()
			{
				return _availableDevices;
			}
		}
	private: // Properties
		cv::Mat _stored_image;
		Platform::String^ jobNumString;
		int jobNumInt;
		WriteableBitmap^ ImageSource = ref new WriteableBitmap(4, 5);

		// Serial comms 
		Platform::Collections::Vector<Platform::Object^>^ _availableDevices;
		Windows::Devices::SerialCommunication::SerialDevice ^_serialPort;
		Windows::Storage::Streams::DataWriter^ _dataWriterObject;
		Windows::Storage::Streams::DataReader^ _dataReaderObject;
		Concurrency::cancellation_token_source* cancellationTokenSource;

	private: // methods
		// Event handlers
		void Page_Loaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void initBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void enterJobNumberBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void oneBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void twoBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void threeBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void fourBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void fiveBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void sixBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void sevenBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void eightBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void nineBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void zeroBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void backspaceBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void clearBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void VEELB::MainPage::screenSaverAnimation(); 
		void redSlider_ValueChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs^ e);
		void greenSlider_ValueChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs^ e);
		void blueSlider_ValueChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs^ e);

		// UI Functions
		void VEELB::MainPage::UpdateImage(const cv::Mat& image);
		void VEELB::MainPage::CameraFeed();
		void returnBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void ScreenSaverGrid_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);
		void exitWebcamBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void toggleHistoryBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void startBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void exitJobNumberBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void sleepBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void settingsWebcamBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		string VEELB::MainPage::convertPlatformStringToStandardString(Platform::String^ inputString);
		Platform::String^ VEELB::MainPage::convertStringToPlatformString(string inputString);
		
		// Serial comms
		vector<int> SeparateIntoDigits(unsigned int value);
		void ListAvailablePorts(void);
		bool IsTracer(Platform::String^ id);
		int CreateChecksum(vector<int>);
		void CancelReadTask(void);
		void CloseDevice(void);
		void Listen();
		Concurrency::task<void> MainPage::WriteAsync(Concurrency::cancellation_token cancellationToken, int jobNum);
		Concurrency::task<void> ReadAsync(Concurrency::cancellation_token cancellationToken);
		Concurrency::task<void> ConnectToSerialDeviceAsync(Windows::Devices::Enumeration::DeviceInformation ^device, Concurrency::cancellation_token cancellationToken);
	};
	
}