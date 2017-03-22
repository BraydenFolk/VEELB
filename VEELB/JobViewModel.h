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
