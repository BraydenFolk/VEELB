#pragma once

using namespace std;

namespace VEELB
{
	public ref class SerialCommsViewModel sealed
	{
	 private:     
	 	Windows::Foundation::Collections::IVector<Platform::Object^>^ _availableDevices;
        Windows::Devices::SerialCommunication::SerialDevice ^_serialPort;
     	Windows::Storage::Streams::DataWriter^ _dataWriterObject;
     	Windows::Storage::Streams::DataReader^ _dataReaderObject;
    	Concurrency::cancellation_token_source* cancellationTokenSource;
		Concurrency::task<void> ConnectToSerialDeviceAsync(Windows::Devices::Enumeration::DeviceInformation ^device, Concurrency::cancellation_token cancellationToken);
		Concurrency::task<void> WriteAsync(Concurrency::cancellation_token cancellationToken, int messageToSend);
		Concurrency::task<void> ReadAsync(Concurrency::cancellation_token cancellationToken);
		void Listen();
		void closeDevice();

	 public:
		static Windows::Foundation::IAsyncOperation<Windows::Devices::Enumeration::DeviceInformationCollection ^> ^ListAvailableSerialDevicesAsync(void);
		void sendJob(int jobNum);
		void ListSerialDevices();
		Windows::Foundation::Collections::IVector<Platform::Object^>^ getAvailableDevices(); 
		
		void ListAvailablePorts(void);
		void CancelReadTask(void);
		int CreateChecksum(Platform::String^ message);
		bool IsTracer(Platform::String^ id);

		void Connect();
		
	};

}