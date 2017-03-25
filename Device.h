/// Device.h
/// <summary>
/// Header file for the Device class
/// </summary>
/// <author> Petra Kujawa </author>

#pragma once


using namespace Windows::Devices::Enumeration;
using namespace std;

namespace VEELB
{
	/// <summary>
	/// A wrapper class for holding DeviceInformation 
	/// </summary>
	public ref class Device sealed
	{
	public:
		Device(Platform::String^ id, DeviceInformation^ deviceInfo);

		property Platform::String^ Id
		{
			Platform::String^ get()
			{
				return _id;
			}
		}
		property DeviceInformation^ DeviceInfo
		{
			DeviceInformation^ get()
			{
				return _deviceInformation;
			}
		}

	private:
		Platform::String^ _id;
		DeviceInformation^ _deviceInformation;
	};

}