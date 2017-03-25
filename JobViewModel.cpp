/// JobViewModel.cpp
/// <summary>
/// Implementation of the JobViewModel class. 
/// This class perfoms the creating of individual jobs, and retreiving the coordinates for crosshair placement
/// </summary>
/// <author> Brayden Folk </author>
/// <author> Petra Kujawa </author>

#include "pch.h"
#include "JobViewModel.h"

using namespace VEELB;
using namespace Windows::ApplicationModel::Background;
using namespace Windows::Foundation;
using namespace Windows::Storage;
using namespace Windows::System::Threading;

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

int JobViewModel::getXPosition()
{
	return xPosition;
}

int JobViewModel::getYPosition()
{
	return yPosition;
}