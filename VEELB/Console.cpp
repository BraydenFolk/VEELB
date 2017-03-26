#include "pch.h"
#include "Console.h"
#include "JobViewModel.h"

using namespace VEELB;

using namespace concurrency;
using namespace Platform;
using namespace Windows::Storage;
using namespace Windows::UI::Xaml;

Console::Console()
{
}

Console::Console(JobViewModel^ job1, Platform::String^ timeStamp1)
{
	job = job1;
	timeStamp = timeStamp1;
}

Platform::String^ Console::getTimeStamp()
{
	return timeStamp;
}

void Console::setJob(JobViewModel^ job1)
{
	job = job1;
}

JobViewModel^ Console::getJob()
{
	return job;
}