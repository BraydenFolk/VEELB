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
		void Run(Windows::ApplicationModel::Background::IBackgroundTaskInstance^ taskInstance);
		void Start(Windows::ApplicationModel::Background::IBackgroundTaskInstance^ taskInstance);
		JobViewModel(int jobNumber);
		JobViewModel();
		int getJobNumber();
		double getXPosition();
		double getYPosition();
		void setData(byte inData);

	private:
		int _jobNumber;
		double _xPosition;
		double _yPosition;
		unsigned int _data;
	};
}