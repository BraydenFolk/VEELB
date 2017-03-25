/// Device.cpp
/// <summary>
/// Implementation of Device class. Creates new instances of Devices.
/// </summary>
/// <author> Petra Kujawa </author>

#include "pch.h"
#include "Device.h"

using namespace VEELB;
using namespace Windows::Devices::Enumeration;


/// <summary>
/// Constructor for the Device class
/// </summary>
Device::Device(Platform::String^ id, DeviceInformation^ deviceInfo)
{
	_id = id;
	_deviceInformation = deviceInfo;
}