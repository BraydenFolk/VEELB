//
// JobViewModel.cpp
// Implementation of the JobViewModel class. 
// This class perfoms the creating of individual jobs, and retreiving the coordinates for crosshair placement
//

#include "pch.h"
#include "JobViewModel.h"

using namespace VEELB;
using namespace Windows::ApplicationModel::Background;
using namespace Windows::Foundation;
using namespace Windows::Storage;
using namespace Windows::System::Threading;


void JobViewModel::Run(IBackgroundTaskInstance^ taskInstance)
{
}


JobViewModel::JobViewModel(int jobNumber1)
{
	jobNumber = jobNumber1;
}


JobViewModel::JobViewModel()
{
}


int JobViewModel::getJobNumber()
{
	return jobNumber;
}


void JobViewModel::Start(IBackgroundTaskInstance^ taskInstance)
{
	// Use the taskInstance->Name and/or taskInstance->InstanceId to determine
	// what background activity to perform. In this sample, all of our
	// background activities are the same, so there is nothing to check.
	auto activity = ref new JobViewModel();
	activity->Run(taskInstance);
}


int JobViewModel::getXPosition()
{
	return xPosition;
}


int JobViewModel::getYPosition()
{
	return yPosition;
}


void JobViewModel::setData(byte inData)
{
	data += inData;
}