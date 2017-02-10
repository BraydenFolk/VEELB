#pragma once
#include "SerialCommsViewModel.h"
using namespace std;

namespace VEELB
{
	public ref class JobViewModel sealed
	{
	public:
		void Run(Windows::ApplicationModel::Background::IBackgroundTaskInstance^ taskInstance);
		void Start(Windows::ApplicationModel::Background::IBackgroundTaskInstance^ taskInstance);
		JobViewModel(int jobNumber, SerialCommsViewModel^ serialViewModel);
		JobViewModel(); 
		int getJobNumber();
		int getXPosition();
		int getYPosition();
		void setDataReceived(Platform::String^ inData);

	private:
		int jobNumber;
		double xPosition;
		double yPosition;
		Platform::String^ dataReceived;
	};
}

