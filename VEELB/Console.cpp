#include "pch.h"
#include "Console.h"
#include "JobViewModel.h"

using namespace VEELB;

using namespace concurrency;
using namespace Platform;
using namespace Windows::Storage;
using namespace Windows::UI::Xaml;

/// <summary>
/// Default constructor
/// </summary>
Console::Console()
{
}

/// <summary>
/// Custom constructor, sets job and timestamp
/// </summary>
Console::Console(JobViewModel^ job1, Platform::String^ timeStamp1)
{
	job = job1;
	timeStamp = timeStamp1;
}

/// <summary>
/// Getter for timestamp
/// </summary>
Platform::String^ Console::getTimeStamp()
{
	return timeStamp;
}

/// <summary>
/// setter for job
/// </summary>
void Console::setJob(JobViewModel^ job1)
{
	job = job1;
}

/// <summary>
/// Getter for job
/// </summary>
JobViewModel^ Console::getJob()
{
	return job;
}