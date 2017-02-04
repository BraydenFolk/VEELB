#include "pch.h"
#include "SerialCommsViewModel.h"
#include "Device.h"
#include "MainPage.xaml.h"
#include "JobViewModel.h"

using namespace VEELB;
using namespace Windows::UI::Xaml;
using namespace std;

void SerialCommsViewModel::ConnectToTracer()
{
	_availableDevices = ref new Platform::Collections::Vector<Platform::Object^>();
	ListAvailablePorts();
}

void SerialCommsViewModel::sendJob(Platform::String^ jobNum)
{
	Device^ selectedDevice = static_cast<Device^>(_availableDevices->GetAt(0));
	Windows::Devices::Enumeration::DeviceInformation ^entry = selectedDevice->DeviceInfo;

//	concurrency::create_task(ConnectToSerialDeviceAsync(entry, cancellationTokenSource->get_token()));
	//_availableDevices = ref new Platform::Collections::Vector<Platform::Object^>();
	//ListAvailablePorts(); // This method makes it break

	//Device^ selectedDevice = static_cast<Device^>(_availableDevices->GetAt(0));
	//Windows::Devices::Enumeration::DeviceInformation ^entry = selectedDevice->DeviceInfo;

	//concurrency::create_task(ConnectToSerialDeviceAsync(entry, cancellationTokenSource->get_token()));

	//WriteAsync(cancellationTokenSource->get_token(), jobNum);
}

/// <summary>
/// Finds all serial devices available on the device and populates a list with the Ids of each device.
/// </summary>
void SerialCommsViewModel::ListAvailablePorts(void)
{
	cancellationTokenSource = new Concurrency::cancellation_token_source();

	// Using asynchronous operation, get a list of serial devices available on this device
	Concurrency::create_task(ListAvailableSerialDevicesAsync()).then([this](Windows::Devices::Enumeration::DeviceInformationCollection ^serialDeviceCollection)
	{
		/**** The program execution does not enter this code and I can't figure out why. Copy and pasted the SerialSample code which worked perfectly*/
		Windows::Devices::Enumeration::DeviceInformationCollection ^_deviceCollection = serialDeviceCollection;
		
		// start with an empty list
		_availableDevices->Clear();

		for (auto &&device : serialDeviceCollection)
		{
			_availableDevices->Append(ref new Device(device->Id, device));
		}
	});
}

Windows::Foundation::Collections::IVector<Platform::Object^>^ SerialCommsViewModel::getAvailableDevices()
{
	return _availableDevices;
}

/// <Summary>
/// Determines if the device Id corresponds to the Tracer or another type of serial device since more
/// devices may be connected to the Pi in the future for the company as they add features to their overall
/// system of making lesnes
bool SerialCommsViewModel::IsTracer(Platform::String^ id)
{
	if (id == "\\")
		return true;
	else
		return false;
}

/// <summary>
/// An asynchronous operation that returns a collection of DeviceInformation objects for all serial devices detected on the device.
/// </summary>
Windows::Foundation::IAsyncOperation<Windows::Devices::Enumeration::DeviceInformationCollection ^> ^SerialCommsViewModel::ListAvailableSerialDevicesAsync(void)
{
	// Construct AQS String for all serial devices on system
	Platform::String ^serialDevices_aqs = Windows::Devices::SerialCommunication::SerialDevice::GetDeviceSelector();

	// Identify all paired devices satisfying query
	return Windows::Devices::Enumeration::DeviceInformation::FindAllAsync(serialDevices_aqs);
}
//
///// <summary>
///// Creates a task chain that attempts connect to a serial device asynchronously. 
///// </summary
//Concurrency::task<void> SerialCommsViewModel::ConnectToSerialDeviceAsync(Windows::Devices::Enumeration::DeviceInformation ^device, Concurrency::cancellation_token cancellationToken)
//{
//	return Concurrency::create_task(Windows::Devices::SerialCommunication::SerialDevice::FromIdAsync(device->Id), cancellationToken)
//		.then([this](Windows::Devices::SerialCommunication::SerialDevice ^serial_device)
//	{
//		try
//		{
//			_serialPort = serial_device;
//
//			Windows::Foundation::TimeSpan _timeOut;
//			_timeOut.Duration = 10000000L;
//
//			// Configure serial settings
//			_serialPort->WriteTimeout = _timeOut;
//			_serialPort->ReadTimeout = _timeOut;
//			_serialPort->BaudRate = 19200;
//			_serialPort->Parity = Windows::Devices::SerialCommunication::SerialParity::None;
//			_serialPort->StopBits = Windows::Devices::SerialCommunication::SerialStopBitCount::One;
//			_serialPort->DataBits = 8;
//			_serialPort->Handshake = Windows::Devices::SerialCommunication::SerialHandshake::None;
//
//			// setup our data reader for handling incoming data
//			_dataReaderObject = ref new Windows::Storage::Streams::DataReader(_serialPort->InputStream);
//			_dataReaderObject->InputStreamOptions = Windows::Storage::Streams::InputStreamOptions::Partial;
//
//			// setup our data writer for handling outgoing data
//			_dataWriterObject = ref new Windows::Storage::Streams::DataWriter(_serialPort->OutputStream);
//
//			// Setting this text will trigger the event handler that runs asynchronously for reading data from the input stream
//			
//			Listen();
//		}
//		catch (Platform::Exception ^ex)
//		{
//			// perform any cleanup needed
//			CloseDevice();
//		}
//	});
//}


/// <summary>
/// Returns the sum of the data bits being sent so a checksum value can be sent to the TRacer for error control
/// </sumary
int SerialCommsViewModel::CreateChecksum(Platform::String^ message)
{
	return 5;
}

/// <summary>
/// Initiates task cancellation
/// </summary
void SerialCommsViewModel::CancelReadTask(void)
{
	cancellationTokenSource->cancel();
}





