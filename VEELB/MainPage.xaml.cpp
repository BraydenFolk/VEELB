
//
// MainPage.xaml.cpp
// Implementation of the MainPage class.
//

#include "pch.h"
#include "MainPage.xaml.h"
#include "SerialCommsViewModel.h"
#include <opencv2\imgproc\types_c.h>
#include <opencv2\imgcodecs\imgcodecs.hpp>
#include <opencv2\core\core.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\objdetect.hpp>
#include <opencv2\videoio.hpp>
#include <opencv2\videoio\cap_winrt.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <Robuffer.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <iomanip>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <codecvt>

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
using namespace std;

void Compare(Mat frame, Mat oldFrame, Mat grayScale);


//our sensitivity value to be used in the threshold() function
const static int SENSITIVITY_VALUE = 20;
//size of blur used to smooth the image to remove possible noise and
//increase the size of the object we are trying to track. (Much like dilate and erode)
const static int BLUR_SIZE = 10;
//we'll have just one object to search for
//and keep track of its position.
int theObject[2] = { 0,0 };
Platform::String^ exceptionHolderText;
//bounding rectangle of the object, we will use the center of this as its position.
cv::Rect objectBoundingRectangle = cv::Rect(0, 0, 0, 0);
int radius = 0;
int detectType = 0;
VideoCapture cam;
const int xPos = 235;
const int yPos = 235;
JobViewModel^ job;


MainPage::MainPage()
{
	InitializeComponent();
	_serialViewModel = ref new SerialCommsViewModel;
	_serialViewModel->ConnectToTracer();
}

// Webcam functions
void cvVideoTask()
{
	cv::Mat frame, oldFrame, tmp, grayScale, canny, canny_output;

	vector<vector<cv::Point> > contours;
	vector<Vec4i> hierarchy;

	cam.open(0);
	while (1)
	{
		// get a new frame from camera - this is non-blocking per spec
		cam >> frame;
		if (!cam.grab()) continue;

		if (frame.rows > 0 && frame.cols > 0)
		{

			Compare(frame, oldFrame, grayScale);
			tmp.copyTo(oldFrame);
			cv::winrt_imshow();
		}

	}
}

int seuil = 10;
void Compare(Mat frame, Mat oldFrame, Mat grayScale)
{
	int colourVal = 0;
	int topX = 0;
	int topY = 0;
	int bottomX = 0;
	int bottomY = 0;
	int first = 0;
	int changed = 0;

	cv::line(frame, cv::Point(xPos, yPos - 10), cv::Point(xPos, yPos + 10), Scalar(0, 204, 0), 3);
	cv::line(frame, cv::Point(xPos - 10, yPos), cv::Point(xPos + 10, yPos), Scalar(0, 204, 0), 3);
	cv::circle(frame, cv::Point(xPos, yPos), radius, Scalar(0, 204, 0), 10, 8, 0);

	for (int i = 0; i < frame.rows; i++)
	{
		for (int j = 0; j < frame.cols; j++)
		{
			Vec3b colour = frame.at<Vec3b>(i, j);

			if (colour.val[0] < 90 && colour.val[1] < 90 && colour.val[2] < 90)
			{
				frame.at<Vec3b>(i, j)[0] = 255;
				frame.at<Vec3b>(i, j)[1] = 0;
				frame.at<Vec3b>(i, j)[2] = 0;

				if (first == 0)
				{
					topX = i;
					topY = j;
					first = 1;
				}

				bottomX = i;
				bottomY = j;
			}
		}
	}

	int tempx = topX + (bottomX - topX) / 2;
	int tempy = (topY + bottomY) / 2;

	radius = (bottomX - topX) / 2;
}

// UI Functions
void VEELB::MainPage::UpdateImage(const cv::Mat& image)
{
	// Create the WriteableBitmap
	WriteableBitmap^ bitmap = ref new WriteableBitmap(image.cols, image.rows);

	// Get access to the pixels
	IBuffer^ buffer = bitmap->PixelBuffer;
	unsigned char* dstPixels = nullptr;

	// Obtain IBufferByteAccess
	ComPtr<IBufferByteAccess> pBufferByteAccess;
	ComPtr<IInspectable> pBuffer((IInspectable*)buffer);
	pBuffer.As(&pBufferByteAccess);

	// Get pointer to pixel bytes
	HRESULT get_bytes = pBufferByteAccess->Buffer(&dstPixels);
	if (get_bytes == S_OK) {
		memcpy(dstPixels, image.data, image.step.buf[1] * image.cols*image.rows);

		// Set the bitmap to the Image element
		imgCV->Source = bitmap;

	}
	else
	{
		printf("Error loading image into buffer\n");
	}
}

// TODO: Test
void VEELB::MainPage::CameraFeed()
{
	Mat src, src_gray, canny_output;
	int thresh = 100; // standard edge colour
	int max_thresh = 255; // max
	vector<vector<cv::Point> > contours;
	vector<Vec4i> hierarchy;

	RNG rng(12345);

	cam.open(0);
	while (1)
	{
		if (!cam.grab()) continue;
		cam >> src;

		if (src.rows == 0 || src.cols == 0)
		{
			continue;
		}

		cvtColor(src, src_gray, CV_BGR2GRAY); // convert to grayscale
		blur(src_gray, src_gray, cv::Size(3, 3)); // blur converted mat

		Canny(src_gray, canny_output, thresh, thresh * 2, 3); // apply canny filter to image
		findContours(canny_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

		Mat drawing = Mat::zeros(canny_output.size(), CV_8UC3);
		for (int i = 0; i < contours.size(); i++)
		{
			Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
			drawContours(drawing, contours, i, color, 2, 8, hierarchy, 0, cv::Point());
		}

		UpdateImage(drawing);
	}
}

// Event handlers
// TODO: Remove
void VEELB::MainPage::Page_Loaded(Object^ sender, RoutedEventArgs^ e)
{
}


void VEELB::MainPage::initBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	MainGrid->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
	WebcamFeedGrid->Visibility = Windows::UI::Xaml::Visibility::Visible;

	winrt_setFrameContainer(imgCV);
	winrt_startMessageLoop(cvVideoTask);

	//CameraFeed();
}

// TODO: finish
void VEELB::MainPage::enterJobNumberBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	MainGrid->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
	JobNumberGrid->Visibility = Windows::UI::Xaml::Visibility::Visible;
}

void VEELB::MainPage::oneBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	jobIdNumTxtBlock->Text += "1";
	jobNumString = jobIdNumTxtBlock->Text;
}

void VEELB::MainPage::twoBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	jobIdNumTxtBlock->Text += "2";
	jobNumString = jobIdNumTxtBlock->Text;
}


void VEELB::MainPage::threeBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	jobIdNumTxtBlock->Text += "3";
	jobNumString = jobIdNumTxtBlock->Text;
}


void VEELB::MainPage::fourBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	jobIdNumTxtBlock->Text += "4";
	jobNumString = jobIdNumTxtBlock->Text;
}


void VEELB::MainPage::fiveBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	jobIdNumTxtBlock->Text += "5";
	jobNumString = jobIdNumTxtBlock->Text;
}


void VEELB::MainPage::sixBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	jobIdNumTxtBlock->Text += "6";
	jobNumString = jobIdNumTxtBlock->Text;
}


void VEELB::MainPage::sevenBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	jobIdNumTxtBlock->Text += "7";
	jobNumString = jobIdNumTxtBlock->Text;
}


void VEELB::MainPage::eightBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	jobIdNumTxtBlock->Text += "8";
	jobNumString = jobIdNumTxtBlock->Text;
}


void VEELB::MainPage::nineBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	jobIdNumTxtBlock->Text += "9";
	jobNumString = jobIdNumTxtBlock->Text;
}


void VEELB::MainPage::zeroBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	jobIdNumTxtBlock->Text += "0";
	jobNumString = jobIdNumTxtBlock->Text;
}

void VEELB::MainPage::backspaceBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	Platform::String^ number = jobIdNumTxtBlock->Text;
	if (number->Length() > 0)
	{
		string jobIdStdString = convertPlatformStringToStandardString(number);
		//jobIdStdString = jobIdStdString.substr(0, jobIdStdString.length - 1);
		jobIdNumTxtBlock->Text = convertStringToPlatformString(jobIdStdString);
		jobNumString = jobIdNumTxtBlock->Text;
	}
}

string VEELB::MainPage::convertPlatformStringToStandardString(Platform::String^ inputString)
{
	wstring tempw(inputString->Begin());
	string jobIdStdString(tempw.begin(), tempw.end() - 1);
	return jobIdStdString;
}

Platform::String^ VEELB::MainPage::convertStringToPlatformString(string inputString)
{
	wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	wstring intermediateForm = converter.from_bytes(inputString);
	Platform::String^ retval = ref new Platform::String(intermediateForm.c_str());
	return retval;
}

void VEELB::MainPage::clearBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	if (jobIdNumTxtBlock->Text == "")
	{
		JobNumberGrid->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
		MainGrid->Visibility = Windows::UI::Xaml::Visibility::Visible;
	}
	else
		jobIdNumTxtBlock->Text = "";
	jobNumString = jobIdNumTxtBlock->Text;
}


void VEELB::MainPage::enterBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	//string jobNum = convertPlatformStringToStandardString(jobIdNumTxtBlock->Text);
	job = ref new JobViewModel(jobIdNumTxtBlock->Text, _serialViewModel);
}


void VEELB::MainPage::ScreenSaverGrid_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
{
	MainGrid->Visibility = Windows::UI::Xaml::Visibility::Visible;
	ScreenSaverGrid->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
}

void VEELB::MainPage::exitWebcamBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	MainGrid->Visibility = Windows::UI::Xaml::Visibility::Visible;
	WebcamFeedGrid->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
}

int main()
{
}