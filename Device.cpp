/// MDevice.cpp
/// <summary>
/// Implementation of Device class. Creates new instances of Devices.
/// </summary>
/// <author> Petra Kujawa </author>

#include "pch.h"
#include "Device.h"

using namespace VEELB;


/// <summary>
/// Constructor for the Device class
/// </summary
Device::Device(Platform::String^ id, Windows::Devices::Enumeration::DeviceInformation^ deviceInfo)
{
	_id = id;
	_deviceInformation = deviceInfo;
}