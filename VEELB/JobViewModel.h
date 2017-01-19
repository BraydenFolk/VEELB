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
	private:
		int jobNumber;
		double xPosition;
		double yPosition;
	};
}
