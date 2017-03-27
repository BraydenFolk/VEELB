#include "pch.h"
#include "JobViewModel.h"

using namespace VEELB;
using namespace Windows::ApplicationModel::Background;
using namespace Windows::Foundation;
using namespace Windows::Storage;
using namespace Windows::System::Threading;


/// <summary>
/// Custom constructor, sets job
/// </summary>
JobViewModel::JobViewModel(int jobNumber1)
{
	jobNumber = jobNumber1;
}

/// <summary>
/// default constructor
/// </summary>
JobViewModel::JobViewModel()
{
}

/// <summary>
/// Getter for job
/// </summary>
int JobViewModel::getJobNumber()
{
	return jobNumber;
}

/// <summary>
/// Getter for x
/// </summary>
int JobViewModel::getXPosition()
{
	return xPosition;
}

/// <summary>
/// setter for x
/// </summary>
void JobViewModel::setXPosition(int xPosition1)
{
	xPosition = (double)xPosition1;
}

/// <summary>
/// Getter for y
/// </summary>
int JobViewModel::getYPosition()
{
	return yPosition;
}

/// <summary>
/// Setter for y
/// </summary>
void JobViewModel::setYPosition(int yPosition1)
{
	yPosition = (double)yPosition1;
}