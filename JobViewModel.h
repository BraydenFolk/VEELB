/// JobViewModel.h
/// <summary>
/// Header file for the JobViewModel class
/// </summary>
/// <author> Brayden Folk </author>
/// <author> Petra Kujawa </author>

#pragma once
namespace VEELB
{
	public ref class JobViewModel sealed
	{
	public:
		JobViewModel(int jobNumber);
		JobViewModel();
		int getJobNumber();
		int getXPosition();
		int getYPosition();
	private:
		int jobNumber;
		double xPosition;
		double yPosition;
	};
}