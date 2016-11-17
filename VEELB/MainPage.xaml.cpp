//
// MainPage.xaml.cpp
// Implementation of the MainPage class.
//

#include "pch.h"
#include "MainPage.xaml.h"
#include <opencv2\imgproc\types_c.h>
#include <opencv2\imgcodecs\imgcodecs.hpp>
#include <opencv2\core\core.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\objdetect.hpp>
#include <opencv2\videoio.hpp>
#include <opencv2\videoio\cap_winrt.hpp>
#include <Robuffer.h>

using namespace VEELB;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

using namespace Windows::UI::Xaml::Media::Imaging;
using namespace Windows::Storage::Streams;
using namespace Microsoft::WRL;
using namespace cv;
using namespace Platform;

void Compare(Mat f, Mat oldF);
VideoCapture cam;

MainPage::MainPage()
{
	InitializeComponent();
}

void cvVideoTask()
{
	Mat frame, oldFrame, tmp;
	cam.open(0);
	while (1)
	{
		// get a new frame from camera - this is non-blocking per spec
		cam >> frame;
		if (!cam.grab()) continue;
		frame.copyTo(tmp);
		Compare(frame, oldFrame);
		tmp.copyTo(oldFrame);
		winrt_imshow();
	}
}

int seuil = 10;
void Compare(Mat f, Mat oldF)
{
	if (oldF.rows == 0)
		return;
	for (int i = 0; i < f.rows; i++)
	{
		for (int j = 0; j < f.cols; j++)
		{
			if (abs(f.at<Vec3b>(i, j)[2] - oldF.at<Vec3b>(i, j)[2]) >seuil &&
				abs(f.at<Vec3b>(i, j)[0] - oldF.at<Vec3b>(i, j)[0]) >seuil&&
				abs(f.at<Vec3b>(i, j)[1] - oldF.at<Vec3b>(i, j)[1]) > seuil)
			{
				f.at<Vec3b>(i, j)[2] = 255;
				f.at<Vec3b>(i, j)[1] = 0;
				f.at<Vec3b>(i, j)[0] = 0;
			}
		}
	}
}

void VEELB::MainPage::Page_Loaded(Object^ sender, RoutedEventArgs^ e)
{
	winrt_setFrameContainer(imgCV);
	winrt_startMessageLoop(cvVideoTask);
}
