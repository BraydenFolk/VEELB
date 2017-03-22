#pragma once
#include "pch.h"
#include "JobViewModel.h"
namespace VEELB
{
	class Console 
	{
	public:
		Console();
		Console::Console(JobViewModel^ job1, Platform::String^ timeStamp1);
		Platform::String^ getTimeStamp();
		JobViewModel^ getJob();
	private:
		JobViewModel^ job;
		Platform::String^ timeStamp;
	};
}