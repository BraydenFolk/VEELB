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