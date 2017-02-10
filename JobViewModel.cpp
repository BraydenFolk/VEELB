#include "pch.h"
#include "JobViewModel.h"
#include "SerialCommsViewModel.h"
#include "MainPage.xaml.h"

using namespace VEELB;
using namespace Windows::ApplicationModel::Background;
using namespace Windows::Foundation;
using namespace Windows::Storage;
using namespace Windows::System::Threading;
using namespace std;


void JobViewModel::Run(IBackgroundTaskInstance^ taskInstance)
{

}

JobViewModel::JobViewModel(int jobNumberIn, SerialCommsViewModel^ serialViewModel)
{
	jobNumber = jobNumberIn;
	serialViewModel->sendJob(jobNumber);
}

JobViewModel::JobViewModel()
{
}

void JobViewModel::Start(IBackgroundTaskInstance^ taskInstance)
{
	// Use the taskInstance->Name and/or taskInstance->InstanceId to determine
	// what background activity to perform. In this sample, all of our
	// background activities are the same, so there is nothing to check.
	auto activity = ref new JobViewModel();
	activity->Run(taskInstance);
}

void JobViewModel::setDataReceived(Platform::String^ inData)
{
	dataReceived = inData;
}

int JobViewModel::getJobNumber()
{
	return jobNumber;
}
int JobViewModel::getXPosition()
{
	return xPosition;
}

int JobViewModel::getYPosition()
{
	return yPosition;
}