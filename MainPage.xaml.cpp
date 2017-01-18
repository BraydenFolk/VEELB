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
#include <opencv2\highgui\highgui.hpp>
#include <Robuffer.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <iomanip>
#include <math.h>
#include <string.h>

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
VideoCapture cam;

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

const int xPos = 235;
const int yPos = 235;

MainPage::MainPage()
{
	InitializeComponent();
}

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
		//imgCV->Source = bitmap;
		
	}
	else {
		printf("Error loading image into buffer\n");
	}
}

void VEELB::MainPage::Page_Loaded(Object^ sender, RoutedEventArgs^ e)
{
}


void VEELB::MainPage::initBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	MainGrid->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
	WebcamFeedGrid->Visibility = Windows::UI::Xaml::Visibility::Visible;
	
	winrt_setFrameContainer(imgCV); 
	winrt_startMessageLoop(cvVideoTask);	
}


void VEELB::MainPage::CameraFeed()
{

}

void VEELB::MainPage::enterJobNumberBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	MainGrid->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
	JobNumberGrid->Visibility = Windows::UI::Xaml::Visibility::Visible;
}

void VEELB::MainPage::oneBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	CvSize imgSize;
	IplImage *image;

	jobIdNumTxtBlock->Text += "1";
	Platform::String^ jobNum = jobIdNumTxtBlock->Text;
	jobIdNumTxtBlock->Text = jobNum;
}

void VEELB::MainPage::twoBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	jobIdNumTxtBlock->Text += "2";
}


void VEELB::MainPage::threeBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	jobIdNumTxtBlock->Text += "3";
}


void VEELB::MainPage::fourBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	jobIdNumTxtBlock->Text += "4";
}


void VEELB::MainPage::fiveBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	jobIdNumTxtBlock->Text += "5";
}


void VEELB::MainPage::sixBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	jobIdNumTxtBlock->Text += "6";
}


void VEELB::MainPage::sevenBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	jobIdNumTxtBlock->Text += "7";
}


void VEELB::MainPage::eightBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	jobIdNumTxtBlock->Text += "8";
}


void VEELB::MainPage::nineBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	jobIdNumTxtBlock->Text += "9";
}


void VEELB::MainPage::zeroBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	jobIdNumTxtBlock->Text += "0";
}


void VEELB::MainPage::backspaceBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
}


void VEELB::MainPage::clearBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	jobIdNumTxtBlock->Text += "";
}


void VEELB::MainPage::Button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{

}

int main() 
{
}
