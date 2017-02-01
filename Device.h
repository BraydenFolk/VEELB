#pragma once
//
//using namespace Windows::Devices::Enumeration;
//using namespace std;
//
//namespace VEELB
//{
//	/// <summary>
//	/// A wrapper class for holding DeviceInformation while being bindable to XAML. 
//	/// </summary>
//	//[Windows::UI::Xaml::Data::Bindable] // in c++, adding this attribute to ref classes enables data binding for more info search for 'Bindable' on the page http://go.microsoft.com/fwlink/?LinkId=254639 
//	public ref class Device sealed
//	{
//	public:
//		Device(Platform::String^ id, Windows::Devices::Enumeration::DeviceInformation^ deviceInfo);
//
//		property Platform::String^ Id
//		{
//			Platform::String^ get()
//			{
//				return _id;
//			}
//		}
//		property Windows::Devices::Enumeration::DeviceInformation^ DeviceInfo
//		{
//			Windows::Devices::Enumeration::DeviceInformation^ get()
//			{
//				return _deviceInformation;
//			}
//		}
//		
//
//	private:
//		Platform::String^ _id;
//		Windows::Devices::Enumeration::DeviceInformation^ _deviceInformation;
//	};
//
//}