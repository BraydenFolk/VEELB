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
		void setXPosition(int xPosition1);
		int getYPosition();
		void setYPosition(int yPosition1);
	private:
		int jobNumber;
		double xPosition;
		double yPosition;
	};
}
