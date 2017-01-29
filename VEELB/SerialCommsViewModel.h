#pragma once

using namespace Platform;

namespace VEELB
{
	public ref class SerialCommsViewModel sealed
	{
	private:
		Platform::Collections::Vector<Platform::Object^>^ _availableDevices;
		Windows::Devices::SerialCommunication::SerialDevice ^_serialPort;
		Windows::Storage::Streams::DataWriter^ _dataWriterObject;
		Windows::Storage::Streams::DataReader^ _dataReaderObject;
		Concurrency::cancellation_token_source* cancellationTokenSource;

		void Listen();
		void ListAvailablePorts(void);
		void CancelReadTask(void);
		void CloseDevice(void);
		int CreateChecksum(Platform::String^ message);
		bool IsTracer(Platform::String^ id);
		Concurrency::task<void> WriteAsync(Concurrency::cancellation_token cancellationToken);
		Concurrency::task<void> ReadAsync(Concurrency::cancellation_token cancellationToken);
		Concurrency::task<void> ConnectToSerialDeviceAsync(Windows::Devices::Enumeration::DeviceInformation ^device, Concurrency::cancellation_token cancellationToken);

	public:
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
	};

}