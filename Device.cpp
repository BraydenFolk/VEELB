//
// Device.cpp
// Implementation of the Device class. 
// This class creates instances of serial Devices
//

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