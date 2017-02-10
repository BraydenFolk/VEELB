#include "pch.h"
#include "SerialCommsViewModel.h"
#include "Device.h"
#include "MainPage.xaml.h"
#include "JobViewModel.h"

using namespace VEELB;
using namespace Windows::UI::Xaml;
using namespace std;

void SerialCommsViewModel::ListSerialDevices()
{
	_availableDevices = ref new Platform::Collections::Vector<Platform::Object^>();
	ListAvailablePorts();
}

/// <summary>
/// Finds all serial devices available on the device and populates a list with the Ids of each device.
/// </summary>
void SerialCommsViewModel::ListAvailablePorts(void)
{
//	_availableDevices = ref new Platform::Collections::Vector<Platform::Object^>();
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


void SerialCommsViewModel::Connect()
{
	Device^ selectedDevice = static_cast<Device^>(_availableDevices->GetAt(0));
	Windows::Devices::Enumeration::DeviceInformation ^entry = selectedDevice->DeviceInfo;

	concurrency::create_task(ConnectToSerialDeviceAsync(entry, cancellationTokenSource->get_token()));
}

/// <summary>
/// Creates a task chain that attempts connect to a serial device asynchronously. 
/// </summary
Concurrency::task<void> SerialCommsViewModel::ConnectToSerialDeviceAsync(Windows::Devices::Enumeration::DeviceInformation ^device, Concurrency::cancellation_token cancellationToken)
{
	return Concurrency::create_task(Windows::Devices::SerialCommunication::SerialDevice::FromIdAsync(device->Id), cancellationToken)
		.then([this](Windows::Devices::SerialCommunication::SerialDevice ^serial_device)
	{
		try
		{
			_serialPort = serial_device;

			Windows::Foundation::TimeSpan _timeOut;
			_timeOut.Duration = 10000000L;

			// Configure serial settings
			_serialPort->WriteTimeout = _timeOut;
			_serialPort->ReadTimeout = _timeOut;
			_serialPort->BaudRate = 19200;
			_serialPort->Parity = Windows::Devices::SerialCommunication::SerialParity::None;
			_serialPort->StopBits = Windows::Devices::SerialCommunication::SerialStopBitCount::One;
			_serialPort->DataBits = 8;
			_serialPort->Handshake = Windows::Devices::SerialCommunication::SerialHandshake::None;

			// setup our data reader for handling incoming data
			_dataReaderObject = ref new Windows::Storage::Streams::DataReader(_serialPort->InputStream);
			_dataReaderObject->InputStreamOptions = Windows::Storage::Streams::InputStreamOptions::Partial;

			// setup our data writer for handling outgoing data
			_dataWriterObject = ref new Windows::Storage::Streams::DataWriter(_serialPort->OutputStream);

			// Setting this text will trigger the event handler that runs asynchronously for reading data from the input stream
			
			//Listen();
		}
		catch (Platform::Exception ^ex)
		{
			// perform any cleanup needed
		//	CloseDevice();
		}
	});
}

void SerialCommsViewModel::sendJob(int jobNum)
{
	if (_serialPort != nullptr)
	{
		try
		{
			//Platform::Array<unsigned char>^ bytes = ref new Platform::Array(jobNum);
			WriteAsync(cancellationTokenSource->get_token(), jobNum);
		}
		catch (Platform::Exception ^ex)
		{
		}
	}
	else
	{
		//status->Text = "Select a device and connect";
	}
}

/// <summary>
/// Returns a task that sends the outgoing data from the sendText textbox to the output stream. 
/// </summary
Concurrency::task<void> SerialCommsViewModel::WriteAsync(Concurrency::cancellation_token cancellationToken, int messageToSend)
{
	_dataWriterObject->WriteByte(messageToSend);

	return concurrency::create_task(_dataWriterObject->StoreAsync(), cancellationToken).then([this](unsigned int bytesWritten)
	{
	});
}


/// <summary>
/// Returns a task that reads in the data from the input stream
/// </summary
Concurrency::task<void> SerialCommsViewModel::ReadAsync(Concurrency::cancellation_token cancellationToken)
{
	unsigned int _readBufferLength = 2048;

	return concurrency::create_task(_dataReaderObject->LoadAsync(_readBufferLength), cancellationToken).then([this](unsigned int bytesRead)
	{
		// start listening again after done with this chunk of incoming data
		Listen();
	});
}

/// <summary>
/// Event handler that starts listening the serial port input
/// </summary
void SerialCommsViewModel::Listen()
{
	try
	{
		if (_serialPort != nullptr)
		{
			cancellationTokenSource = new Concurrency::cancellation_token_source();

			// calling task.wait() is not allowed in Windows Runtime STA (Single Threaded Apartment) threads due to blocking the UI.
			concurrency::create_task(ReadAsync(cancellationTokenSource->get_token()));
		}
	}
	catch (Platform::Exception ^ex)
	{
		if (ex->GetType()->FullName == "TaskCanceledException")
		{
			closeDevice();
		}
	}
}

/// <summary>
/// Closes the comport currently connected
/// </summary
void SerialCommsViewModel::closeDevice(void)
{
	delete(_dataReaderObject);
	_dataReaderObject = nullptr;

	delete(_dataWriterObject);
	_dataWriterObject = nullptr;

	delete(_serialPort);
	_serialPort = nullptr;
}



/// <summary>
/// Returns the sum of the data bits being sent so a checksum value can be sent to the TRacer for error control
/// </sumary
int SerialCommsViewModel::CreateChecksum(Platform::String^ message)
{
	return 5;
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
/// Initiates task cancellation
/// </summary
void SerialCommsViewModel::CancelReadTask(void)
{
	cancellationTokenSource->cancel();
}





