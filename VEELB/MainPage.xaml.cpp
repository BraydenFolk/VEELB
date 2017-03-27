//
// MainPage.xaml.cpp
// Implementation of the MainPage class.
//

#include "pch.h"
#include "MainPage.xaml.h"
#include "Console.h"
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
using namespace Windows::UI::Popups;

using namespace Windows::Storage;

using namespace Windows::UI::Xaml::Media::Imaging;
using namespace Windows::Storage::Streams;
using namespace Microsoft::WRL;
using namespace cv;
using namespace Platform;
using namespace std;

void Compare(Mat frame, Mat oldFrame, Mat grayScale);
cv::String convertPlatformToCV(Platform::String^ input);

VideoCapture cam;
int xPos = 0;
int yPos = 0;
int redSldr = 0;
int greenSldr = 0;
int blueSldr = 0;
int thicknessSldr = 0;

JobViewModel^ job;
JobViewModel^ pendingjob;
int pendingConsoleIndex = 0;
int jobCtr = 0;
int consoleSelectedIndex = 0;

bool onExit = false;
bool canSave = false;
bool firstInit = true;
auto itemCollection = ref new Platform::Collections::Vector<Object^>();

MainPage::MainPage()
{
	InitializeComponent();

	Status->Text = "Initializing...";
	progBar->Value = 10;

	_availableDevices = ref new Platform::Collections::Vector<Platform::Object^>();
	ListAvailablePorts();
	progBar->Value = 50;
	CreateFile(1);
	CreateFile(2);
	progBar->Value = 100;
	ReadTextFromFile(1);
	Status->Text = "Initialized!";
	loadButton->IsEnabled = false;

	consoleCtr = 0;
}

// Webcam functions
/// <summary>
/// Background message loop task for webcam feed, allows for functionality such as initializing the webcam,
///		retrieving frames from the webcam and passes frames to compare which is where the main work is being done.
/// <param name=”paramName”> void </param> 
/// <returns> void </returns>
/// </summary>
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
			frame.copyTo(tmp);
			Compare(frame, oldFrame, grayScale);
			tmp.copyTo(oldFrame);
			
			cv::winrt_imshow();
		}

		if (onExit)
		{
			break;
		}
		
	}

	cam.release();
}

struct darkCoords
{
	int row;
	int col;
};

int seuil = 20;
/// <summary>
/// This function takes in the current and last frames, compares them to eachother to determine if there is motion, if there is 
///		the image processing and lens detection will not be conducted due to the fact that there exists unneccesary and harmful
///		pixels in the frames. The image processing portion of the function detects the three dots on the lens and saves the coordinates,
///		so that the crosshair can be placed connecting the three dots. Also, if the location of the midpoint of the lens is within +/- 5
///		pixels, it is deemed correctly located.
/// <param name=”frame”> current webcam frame </param> 
/// <param name = ”oldFrame”>Previous webcam frame< / param>
/// <param name = ”frame”> grayscale version of webcam < / param>
/// <returns> current updated frame </returns>
/// </summary>
void Compare(Mat frame, Mat oldFrame, Mat grayScale)
{
	Mat canny, gray;
	int colourVal = 0;
	int topX = 0;
	int bottomX = 0;
	int leftY = 0;
	int rightY = 0;
	int first = 0;

	double blue = (double)blueSldr;
	double green = (double)greenSldr;
	double red = (double)redSldr;
	double thickness = (double)thicknessSldr;	

	int redLocked = 255;
	int greenLocked = 0;
	int blueLocked = 0;
	int compareCtr = 0;

	bool motion = false;

	if (oldFrame.rows == 0)
		return;
	for (int i = 0; i < frame.rows; i++)
	{
		for (int j = 0; j < frame.cols; j++)
		{
			if (abs(frame.at<cv::Vec3b>(i, j)[2] - oldFrame.at<cv::Vec3b>(i, j)[2]) >seuil &&
				abs(frame.at<cv::Vec3b>(i, j)[0] - oldFrame.at<cv::Vec3b>(i, j)[0]) >seuil&&
				abs(frame.at<cv::Vec3b>(i, j)[1] - oldFrame.at<cv::Vec3b>(i, j)[1]) > seuil)
			{
				compareCtr++;
			}
		}
	}

	if (compareCtr > 1000)
	{
		motion = true;
	}

	if (!motion)
	{
		try
		{
			// starts top-left
			for (int i = 0; i < frame.rows; i++)
			{
				for (int j = 0; j < frame.cols; j++)
				{
					Vec3b colour = frame.at<Vec3b>(i, j);
					bool edge = true;

					if (colour.val[0] < 70 && colour.val[1] < 70 && colour.val[2] < 70)
					{
						frame.at<Vec3b>(i, j)[0] = 255;
						frame.at<Vec3b>(i, j)[1] = 0;
						frame.at<Vec3b>(i, j)[2] = 0;

						if (first == 0)
						{
							topX = i;
							leftY = j;
							first = 1;
						}
						bottomX = i;
						rightY = j;
					}
				}
			}
		}
		catch (Platform::Exception^ e)
		{
			Platform::String^ temp = e->Message;
		}
		
		cv::line(frame, cv::Point(0, 0), cv::Point(frame.cols, 0), Scalar(red, green, blue), thickness); // Template line

		int midX = (topX + bottomX) / 2;	// lens midpoint
		int midY = (leftY + rightY) / 2;

		cv::Point pt1(leftY, topX + 5);
		cv::Point pt2(rightY, bottomX - 5);

		cv::line(frame, cv::Point(midY, midX + 25), cv::Point(midY, midX - 25), Scalar(red, green, blue), thickness); // lens crosshair
		cv::line(frame, pt1, pt2, Scalar(red, green, blue), thickness);
		cv::circle(frame, cv::Point(midY, midX), thickness, Scalar(150, 255, 0), 4, 8, 0);

		if (abs(midY - xPos) < 5 && abs(midX - yPos) < 5)
		{
			redLocked = 0;
			greenLocked = 255;
			blueLocked = 0;
		}

		Platform::String^ lensLocationText = "Lens Location (" + midX.ToString() + ", " + midY.ToString() + ")";
		cv::String lensLocationTextS = convertPlatformToCV(lensLocationText); // location text
		cv::putText(frame, lensLocationTextS, cv::Point(15, 420), FONT_HERSHEY_COMPLEX_SMALL, 0.8, Scalar(0, 0, 0), 1.25, CV_AA);
	}

	// Target location
	cv::line(frame, cv::Point(xPos, yPos - 25), cv::Point(xPos, yPos + 25), Scalar(redLocked, greenLocked, blueLocked), thickness);
	cv::line(frame, cv::Point(xPos - 25, yPos), cv::Point(xPos + 25, yPos), Scalar(redLocked, greenLocked, blueLocked), thickness);
	
	Platform::String^ locationText = "Target Location (" + xPos.ToString() + ", " + yPos.ToString() + ")";
	cv::String locationTextS = convertPlatformToCV(locationText); // location text
	cv::putText(frame, locationTextS, cv::Point(15, 400), FONT_HERSHEY_COMPLEX_SMALL, 0.8, Scalar(0, 0, 0), 1.25, CV_AA);
}

/// <summary>
/// Converts platform strings to open cv strings for location text
/// <param name=”input”> input text </param> 
/// <returns> open cv string </returns>
/// </summary>
cv::String convertPlatformToCV(Platform::String^ input)
{
	wstring tempw(input->Begin());
	string temps(tempw.begin(), tempw.end());
	cv::String output(temps);
	return output;
}

// UI Functions
/// <summary>
/// Displays the correct UI grids once the webcam is exited, also setting the onExit so the webcam feed can be disposed
///		as well as saving of the settings to the config file.
/// <param name=”sender”> sender object </param> 
/// <param name=”e”> xaml routed event </param> 
/// <returns> void </returns>
/// </summary>
void VEELB::MainPage::exitWebcamBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	onExit = true;

	WebcamFeedGrid->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
	MainGrid->Visibility = Windows::UI::Xaml::Visibility::Visible;

	Platform::String^ txt = redSldr.ToString() + " " + greenSldr.ToString() + " " + blueSldr.ToString() + " " + thicknessSldr.ToString();

	if (WriteTextToFile(1, txt)) {} // success
}

/// <summary>
/// Displays a custom message box
/// <param name=”customMessage”> message box statement </param>  
/// <returns> void </returns>
/// </summary>
void VEELB::MainPage::CustomMessageDialog(Platform::String^ customMessage)
{
	// Create the message dialog and set its content and title
	auto messageDialog = ref new MessageDialog(customMessage, "Initialization not complete");

	// Add commands and set their callbacks
	messageDialog->Commands->Append(ref new UICommand("Continue", ref new UICommandInvokedHandler([this](IUICommand^ command)
	{
	})));
	messageDialog->Commands->Append(ref new UICommand("Exit", ref new UICommandInvokedHandler([this](IUICommand^ command)
	{
	})));

	// Set the command that will be invoked by default
	//messageDialog->DefaultCommandIndex = 1;

	// Show the message dialog
	messageDialog->ShowAsync();
}

// Event handlers
/// <summary>
/// Initializes the webcam feed and retrieves the job's target location, and upon first init, parses the config file.
/// <param name=”sender”> sender object </param> 
/// <param name=”e”> xaml routed event </param> 
/// <returns> void </returns>
/// </summary>
void VEELB::MainPage::initBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	Platform::String^ temp;
	try
	{
		if (locationTextBlk->Text == "")
		{
			Status->Text = "Location n/a...";
		}

		if (job != nullptr)
		{
			xPos = job->getXPosition();
			yPos = job->getYPosition();
		}

		if (firstInit)
		{
			Status->Text = "Loading config...";
			progBar->Value = 50;

			Platform::String^ params = configTextBlk->Text;
			std::wstring str = params->Data();

			int spacePos = str.find(' ');
			std::wstring strLeft = str.substr(0, spacePos);
			int tempLen = str.length() - spacePos;
			str = str.substr(spacePos + 1, tempLen);

			redSlider->Value = (double)(_wtol(strLeft.data()));
			progBar->Value += 10;

			spacePos = str.find(' ');
			strLeft = str.substr(0, spacePos);
			tempLen = str.length() - spacePos;
			str = str.substr(spacePos + 1, tempLen);

			greenSlider->Value = (double)(_wtol(strLeft.data()));
			progBar->Value += 10;

			spacePos = str.find(' ');
			strLeft = str.substr(0, spacePos);
			tempLen = str.length() - spacePos;
			str = str.substr(spacePos + 1, tempLen);

			blueSlider->Value = (double)(_wtol(strLeft.data()));
			progBar->Value += 10;

			spacePos = str.find(' ');
			strLeft = str.substr(0, spacePos);
			tempLen = str.length() - spacePos;
			str = str.substr(spacePos + 1, tempLen);

			thicknessSlider->Value = (double)(_wtol(strLeft.data()));
			progBar->Value += 10;

			Status->Text = "Config loaded!";
			firstInit = false;
		}
	}
	catch (Platform::Exception^ e)
	{
		temp = e->Message;
	}

	progBar->Value = 100;

	MainGrid->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
	WebcamFeedGrid->Visibility = Windows::UI::Xaml::Visibility::Visible;

	winrt_setFrameContainer(imgCV); 
	winrt_startMessageLoop(cvVideoTask);

	onExit = false;
}

/// <summary>
/// screenSaverAnimation
/// <returns> void </returns>
/// </summary>
void VEELB::MainPage::screenSaverAnimation()
{
	MainGrid->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
	ScreenSaverGrid->Visibility = Windows::UI::Xaml::Visibility::Visible;	
	Sleep(3000);

	while (1)
	{
		for (int i = 0; i < 3; i++)
		{
			switch (i)
			{
				case 1:
					screenSaverImg->VerticalAlignment = Windows::UI::Xaml::VerticalAlignment::Top;
					break;
				case 2:
					screenSaverImg->VerticalAlignment = Windows::UI::Xaml::VerticalAlignment::Center;
					break;
				case 3:
					screenSaverImg->VerticalAlignment = Windows::UI::Xaml::VerticalAlignment::Bottom;
					break;
			}
			for (int j = 0; j < 3; j++)
			{
				switch (j)
				{
					case 1:
						screenSaverImg->HorizontalAlignment = Windows::UI::Xaml::HorizontalAlignment::Left;
						break;
					case 2:
						screenSaverImg->HorizontalAlignment = Windows::UI::Xaml::HorizontalAlignment::Center;
						break;
					case 3:
						screenSaverImg->HorizontalAlignment = Windows::UI::Xaml::HorizontalAlignment::Right;
						break;
				}
				Sleep(2000);
			}
		}
	}
}


/// <summary>
/// Button click event for settings grid, inits webcam feed with splitter open
/// <param name=”sender”> sender object </param> 
/// <param name=”e”> xaml routed event </param> 
/// <returns> void </returns>
/// </summary>
void VEELB::MainPage::settingsBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	initBtn_Click(sender, e);
	WebcamSplitter->IsPaneOpen = true;
}

/// <summary>
/// Button click event, displays the correct UI Grids
/// <param name=”sender”> sender object </param> 
/// <param name=”e”> xaml routed event </param> 
/// <returns> void </returns>
/// </summary>
void VEELB::MainPage::enterJobNumberBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	MainGrid->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
	JobNumberGrid->Visibility = Windows::UI::Xaml::Visibility::Visible;
}

/// <summary>
/// Validates entered job number to ensure that it is <= 6 digits
/// <param name=”sender”> sender object </param> 
/// <param name=”e”> xaml routed event </param> 
/// <returns> void </returns>
/// </summary>
void VEELB::MainPage::validate(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	if (jobIdNumTxtBlock->Text->Length() == 6)
	{
		backspaceBtn_Click(sender, e);
	}
}

/// <summary>
/// Button click event for job number screen
/// <param name=”sender”> sender object </param> 
/// <param name=”e”> xaml routed event </param> 
/// <returns> void </returns>
/// </summary>
void VEELB::MainPage::oneBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	validate(sender, e);
	jobIdNumTxtBlock->Text += "1";
	jobNumString = jobIdNumTxtBlock->Text;
	if (jobIdNumTxtBlock->Text->Length() == 6) jobIdNumErrorTxtBlock->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
}

/// <summary>
/// Button click event for job number screen
/// <param name=”sender”> sender object </param> 
/// <param name=”e”> xaml routed event </param> 
/// <returns> void </returns>
/// </summary>
void VEELB::MainPage::twoBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	validate(sender, e);
	jobIdNumTxtBlock->Text += "2";
	jobNumString = jobIdNumTxtBlock->Text;
	if (jobIdNumTxtBlock->Text->Length() == 6) jobIdNumErrorTxtBlock->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
}

/// <summary>
/// Button click event for job number screen
/// <param name=”sender”> sender object </param> 
/// <param name=”e”> xaml routed event </param> 
/// <returns> void </returns>
/// </summary>
void VEELB::MainPage::threeBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	validate(sender, e);
	jobIdNumTxtBlock->Text += "3";
	jobNumString = jobIdNumTxtBlock->Text;
	if (jobIdNumTxtBlock->Text->Length() == 6) jobIdNumErrorTxtBlock->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
}

/// <summary>
/// Button click event for job number screen
/// <param name=”sender”> sender object </param> 
/// <param name=”e”> xaml routed event </param> 
/// <returns> void </returns>
/// </summary>
void VEELB::MainPage::fourBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	validate(sender, e);
	jobIdNumTxtBlock->Text += "4";
	jobNumString = jobIdNumTxtBlock->Text;
	if (jobIdNumTxtBlock->Text->Length() == 6) jobIdNumErrorTxtBlock->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
}

/// <summary>
/// Button click event for job number screen
/// <param name=”sender”> sender object </param> 
/// <param name=”e”> xaml routed event </param> 
/// <returns> void </returns>
/// </summary>
void VEELB::MainPage::fiveBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	validate(sender, e);
	jobIdNumTxtBlock->Text += "5";
	jobNumString = jobIdNumTxtBlock->Text;
	if (jobIdNumTxtBlock->Text->Length() == 6) jobIdNumErrorTxtBlock->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
}

/// <summary>
/// Button click event for job number screen
/// <param name=”sender”> sender object </param> 
/// <param name=”e”> xaml routed event </param> 
/// <returns> void </returns>
/// </summary>
void VEELB::MainPage::sixBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	validate(sender, e);
	jobIdNumTxtBlock->Text += "6";
	jobNumString = jobIdNumTxtBlock->Text;
	if (jobIdNumTxtBlock->Text->Length() == 6) jobIdNumErrorTxtBlock->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
}

/// <summary>
/// Button click event for job number screen
/// <param name=”sender”> sender object </param> 
/// <param name=”e”> xaml routed event </param> 
/// <returns> void </returns>
/// </summary>
void VEELB::MainPage::sevenBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	validate(sender, e);
	jobIdNumTxtBlock->Text += "7";
	jobNumString = jobIdNumTxtBlock->Text;
	if (jobIdNumTxtBlock->Text->Length() == 6) jobIdNumErrorTxtBlock->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
}

/// <summary>
/// Button click event for job number screen
/// <param name=”sender”> sender object </param> 
/// <param name=”e”> xaml routed event </param> 
/// <returns> void </returns>
/// </summary>
void VEELB::MainPage::eightBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	validate(sender, e);
	jobIdNumTxtBlock->Text += "8";
	jobNumString = jobIdNumTxtBlock->Text;
	if (jobIdNumTxtBlock->Text->Length() == 6) jobIdNumErrorTxtBlock->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
}

/// <summary>
/// Button click event for job number screen
/// <param name=”sender”> sender object </param> 
/// <param name=”e”> xaml routed event </param> 
/// <returns> void </returns>
/// </summary>
void VEELB::MainPage::nineBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	validate(sender, e);
	jobIdNumTxtBlock->Text += "9";
	jobNumString = jobIdNumTxtBlock->Text;
	if (jobIdNumTxtBlock->Text->Length() == 6) jobIdNumErrorTxtBlock->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
}

/// <summary>
/// Button click event for job number screen
/// <param name=”sender”> sender object </param> 
/// <param name=”e”> xaml routed event </param> 
/// <returns> void </returns>
/// </summary>
void VEELB::MainPage::zeroBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	validate(sender, e);
	jobIdNumTxtBlock->Text += "0";
	jobNumString = jobIdNumTxtBlock->Text;
	if (jobIdNumTxtBlock->Text->Length() == 6) jobIdNumErrorTxtBlock->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
}

/// <summary>
/// Job number backspace button, and checks if it is safe to backspace
/// <param name=”sender”> sender object </param> 
/// <param name=”e”> xaml routed event </param> 
/// <returns> void </returns>
/// </summary>
void VEELB::MainPage::backspaceBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	Platform::String^ number = jobIdNumTxtBlock->Text;

	if (number->Length() > 0)
	{
		string jobIdStdString = convertPlatformStringToStandardString(number);
		jobIdNumTxtBlock->Text = convertStringToPlatformString(jobIdStdString);
		jobNumString = jobIdNumTxtBlock->Text;
	}
}

/// <summary>
/// converts platform string to std
/// <param name=”inputString”> text to convert </param> 
/// <returns> standard string of input text </returns>
/// </summary>
string VEELB::MainPage::convertPlatformStringToStandardString(Platform::String^ inputString)
{
	wstring tempw(inputString->Begin());
	string jobIdStdString(tempw.begin(), tempw.end() - 1);
	return jobIdStdString;

}

/// <summary>
/// converts std string to platform string
/// <param name=”inputString”> text to convert </param> 
/// <returns> standard string of input text </returns>
/// </summary>
Platform::String^ VEELB::MainPage::convertStringToPlatformString(string inputString)
{
	wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	wstring intermediateForm = converter.from_bytes(inputString);
	Platform::String^ retval = ref new Platform::String(intermediateForm.c_str());
	return retval;
}

/// <summary>
/// Button click event for job number screen
/// <param name=”sender”> sender object </param> 
/// <param name=”e”> xaml routed event </param> 
/// <returns> void </returns>
/// </summary>
void VEELB::MainPage::clearBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	jobIdNumTxtBlock->Text = "";
	jobNumString = jobIdNumTxtBlock->Text;
}

/// <summary>
/// Button click event for job number screen
///	Creates a console entry, with associated job for entered job number, validates job number and sends job number to tracer
/// <param name=”sender”> sender object </param> 
/// <param name=”e”> xaml routed event </param> 
/// <returns> void </returns>
/// </summary>
void MainPage::returnBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	Console consoleEntry;

	if (jobIdNumTxtBlock->Text->Length() != 6)
	{
		jobIdNumErrorTxtBlock->Visibility = Windows::UI::Xaml::Visibility::Visible;
		return;
	}

	jobNumInt = _wtoi(jobNumString->Data());
	job = ref new JobViewModel(jobNumInt);
	mainGridJobNumberTxtBlk->Text = "Current Job number: " + jobNumString;
	
	JobNumberGrid->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
	MainGrid->Visibility = Windows::UI::Xaml::Visibility::Visible;

	TextBlock^ txtBlock = ref new TextBlock();
	ListBoxItem^ lbItem = ref new ListBoxItem();

	Windows::Globalization::Calendar^ c = ref new Windows::Globalization::Calendar;
	Platform::String^ dateTimeNow = c->Year.ToString() + "-" + c->Month + "-" + c->Day + " " + c->Hour + ":" + c->Minute + ":" + c->Second;

	txtBlock->Text = "  Job: " + jobNumString + "                                                                    " + dateTimeNow;

	lbItem->Content = txtBlock;
	itemCollection->Append(lbItem);
	ConsoleListBox->ItemsSource = itemCollection;

	consoleEntry = Console(job, dateTimeNow);
	consoleEntries[jobCtr] = consoleEntry;
	ConsoleListBox->SelectedIndex = jobCtr;

	jobCtr++;

	loadButton->IsEnabled = true;

	if (_serialPort != nullptr)
	{
		try
		{
			if (jobIdNumTxtBlock->Text->Length() > 0)
			{
				Status->Text = "Writing job number...";
				progBar->Value = 10;
				WriteAsync(cancellationTokenSource->get_token(), jobNumInt);
			}
			else
			{
			}
		}
		catch (Platform::Exception ^ex)
		{
		}
	}
	else
	{
	}
}

/// <summary>
/// selection changed for console list box
/// <param name=”sender”> sender object </param> 
/// <param name=”e”> xaml routed event </param> 
/// <returns> void </returns>
/// </summary>
void VEELB::MainPage::ConsoleListBox_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e)
{
	ConsoleListBox->ScrollIntoView(ConsoleListBox->SelectedIndex);
	consoleSelectedIndex = ConsoleListBox->SelectedIndex;
}

/// <summary>
/// Loads previous job
/// <param name=”sender”> sender object </param> 
/// <param name=”e”> xaml routed event </param> 
/// <returns> void </returns>
/// </summary>
void VEELB::MainPage::loadButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	job = consoleEntries[consoleSelectedIndex].getJob();

	mainGridJobNumberTxtBlk->Text = "Current Job number: " + job->getJobNumber().ToString();
}

int main() 
{
}

/// <summary>
/// tap event for screen saver -- not yet implemented
/// <param name=”sender”> sender object </param> 
/// <param name=”e”> xaml routed event </param> 
/// <returns> void </returns>
/// </summary>
void VEELB::MainPage::ScreenSaverGrid_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
{
	ScreenSaverGrid->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
	MainGrid->Visibility = Windows::UI::Xaml::Visibility::Visible;
}

/// <summary>
/// toggles visibility of history list box
/// <param name=”sender”> sender object </param> 
/// <param name=”e”> xaml routed event </param> 
/// <returns> void </returns>
/// </summary>
void VEELB::MainPage::toggleHistoryBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	if (historyGrid->Visibility == Windows::UI::Xaml::Visibility::Visible)
	{
		historyGrid->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
	}
	else
	{
		historyGrid->Visibility = Windows::UI::Xaml::Visibility::Visible;
	}
}

/// <summary>
/// Button click event for start splash screen - selects USB serial device, creates necessary tasks, sets correct UI Grid, reads config
/// <param name=”sender”> sender object </param> 
/// <param name=”e”> xaml routed event </param> 
/// <returns> void </returns>
/// </summary>
void VEELB::MainPage::startBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	Device^ selectedDevice = static_cast<Device^>(_availableDevices->GetAt(0));
	Windows::Devices::Enumeration::DeviceInformation ^entry = selectedDevice->DeviceInfo;

	concurrency::create_task(ConnectToSerialDeviceAsync(entry, cancellationTokenSource->get_token()));

	SplashScreenGrid->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
	MainGrid->Visibility = Windows::UI::Xaml::Visibility::Visible;

	ProgressGrid->Visibility = Windows::UI::Xaml::Visibility::Visible;

	ReadTextFromFile(1);
}

/// <summary>
/// Button click event for job number screen, sets proper UI Grid
/// <param name=”sender”> sender object </param> 
/// <param name=”e”> xaml routed event </param> 
/// <returns> void </returns>
/// </summary>
void VEELB::MainPage::exitJobNumberBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	JobNumberGrid->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
	MainGrid->Visibility = Windows::UI::Xaml::Visibility::Visible;
}

/// <summary>
/// Button click event -- not yet implemented
/// <param name=”sender”> sender object </param> 
/// <param name=”e”> xaml routed event </param> 
/// <returns> void </returns>
/// </summary>
void VEELB::MainPage::sleepBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	MainGrid->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
	ScreenSaverGrid->Visibility = Windows::UI::Xaml::Visibility::Visible;
	screenSaverImg->Visibility = Windows::UI::Xaml::Visibility::Visible;

	screenSaverAnimation();
}

// Settings flyout 
/// <summary>
/// Button click event toggles splitter visibility
/// <param name=”sender”> sender object </param> 
/// <param name=”e”> xaml routed event </param> 
/// <returns> void </returns>
/// </summary>
void MainPage::settingsWebcamBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	WebcamSplitter->IsPaneOpen = !WebcamSplitter->IsPaneOpen;
}

/// <summary>
/// Updates lens crosshair colour
/// <param name=”sender”> sender object </param> 
/// <param name=”e”> RangeBaseValueChangedEventArgs </param> 
/// <returns> void </returns>
/// </summary>
void VEELB::MainPage::redSlider_ValueChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs^ e)
{
	if (redSlider->Value > 255) redSlider->Value = 255; // touch slider is a little wierd when at max
	redSldr = redSlider->Value;
}

/// <summary>
/// Updates lens crosshair colour
/// <param name=”sender”> sender object </param> 
/// <param name=”e”> RangeBaseValueChangedEventArgs </param> 
/// <returns> void </returns>
/// </summary>
void VEELB::MainPage::greenSlider_ValueChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs^ e)
{
	if (greenSlider->Value > 255) greenSlider->Value = 255; // touch slider is a little wierd when at max
	greenSldr = greenSlider->Value;
}

/// <summary>
/// Updates lens crosshair colour
/// <param name=”sender”> sender object </param> 
/// <param name=”e”> RangeBaseValueChangedEventArgs </param> 
/// <returns> void </returns>
/// </summary>
void VEELB::MainPage::blueSlider_ValueChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs^ e)
{
	if (blueSlider->Value > 255) blueSlider->Value = 255; // touch slider is a little wierd when at max
	blueSldr = blueSlider->Value;
}

/// <summary>
/// Updates lens crosshair thickness
/// <param name=”sender”> sender object </param> 
/// <param name=”e”> RangeBaseValueChangedEventArgs </param> 
/// <returns> void </returns>
/// </summary>
void VEELB::MainPage::thicknessSlider_ValueChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs^ e)
{
	if (thicknessSlider->Value > 8) thicknessSlider->Value = 8; // touch slider is a little wierd when at max
	thicknessSldr = thicknessSlider->Value;
}

// Serial comms
// UI toggle
/// <summary>
/// Either connects or disconnects serial comms depending on the present state.
/// <param name=”sender”> sender object </param> 
/// <param name=”e”> xaml routed event </param> 
/// <returns> void </returns>
/// </summary>
void VEELB::MainPage::toggleSerialBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	if (_serialPort != nullptr)
	{
		CancelReadTask();
		CloseDevice();
		ListAvailablePorts();
		Status->Text = "Serial Disconnected!";
		ToggleEnableMainGridBtns();
	}
	else
	{
		Device^ selectedDevice = static_cast<Device^>(_availableDevices->GetAt(0));
		Windows::Devices::Enumeration::DeviceInformation ^entry = selectedDevice->DeviceInfo;

		concurrency::create_task(ConnectToSerialDeviceAsync(entry, cancellationTokenSource->get_token()));
		Status->Text = "Serial Connected!";
		ToggleEnableMainGridBtns();
	}
}

/// <summary>
/// When serial comms is disconnected, job number screen is disabled
/// <returns> void </returns>
/// </summary>
void VEELB::MainPage::ToggleEnableMainGridBtns()
{
	enterJobNumberBtn->IsEnabled = !enterJobNumberBtn->IsEnabled;
}

/// <summary>
/// Finds all serial devices available on the device and populates a list with the Ids of each device.
/// <returns> void </returns>
/// </summary>
void MainPage::ListAvailablePorts(void)
{
	cancellationTokenSource = new Concurrency::cancellation_token_source();
	// Using asynchronous operation, get a list of serial devices available on this device
	
		Concurrency::create_task(ListAvailableSerialDevicesAsync()).then([this](Windows::Devices::Enumeration::DeviceInformationCollection ^serialDeviceCollection)
		{
			/**** The program execution does not enter this code and I can't figure out why. Copy and pasted the SerialSample code which worked perfectly*/
			Windows::Devices::Enumeration::DeviceInformationCollection ^_deviceCollection = serialDeviceCollection;

			// start with an empty list
			_availableDevices->Clear();

			for (auto &&device : serialDeviceCollection)
			{
				_availableDevices->Append(ref new Device(device->Id, device));
			}
		});
}

/// <summary>
/// An asynchronous operation that returns a collection of DeviceInformation objects for all serial devices detected on the device.
/// <returns> DeviceInformationCollection </returns>
/// </summary>
Windows::Foundation::IAsyncOperation<Windows::Devices::Enumeration::DeviceInformationCollection ^> ^MainPage::ListAvailableSerialDevicesAsync(void)
{
	// Construct AQS String for all serial devices on system
	Platform::String ^serialDevices_aqs = Windows::Devices::SerialCommunication::SerialDevice::GetDeviceSelector();

	// Identify all paired devices satisfying query
	return Windows::Devices::Enumeration::DeviceInformation::FindAllAsync(serialDevices_aqs);
}

/// <summary>
/// Creates a task chain that attempts connect to a serial device asynchronously. 
/// <param name=”device1”> current serial device </param> 
/// <param name=”cancellationToken”> token to cancel transaction </param> 
/// <returns> concurrent task </returns>
/// </summary
Concurrency::task<void> MainPage::ConnectToSerialDeviceAsync(Windows::Devices::Enumeration::DeviceInformation ^device1, Concurrency::cancellation_token cancellationToken)
{
	return Concurrency::create_task(Windows::Devices::SerialCommunication::SerialDevice::FromIdAsync(device1->Id), cancellationToken)
		.then([this](Windows::Devices::SerialCommunication::SerialDevice ^serial_device)
	{
		try
		{
			//_serialPort = ref Windows::Devices::SerialCommunication::SerialDevice::FromIdAsync(comPortId);

			_serialPort = serial_device;

			Windows::Foundation::TimeSpan _timeOut;
			_timeOut.Duration = 10000000L;

			// Configure serial settings
			_serialPort->WriteTimeout = _timeOut;
			_serialPort->ReadTimeout = _timeOut;
			_serialPort->BaudRate = 9600;
			_serialPort->Parity = Windows::Devices::SerialCommunication::SerialParity::None;
			_serialPort->StopBits = Windows::Devices::SerialCommunication::SerialStopBitCount::One;
			_serialPort->DataBits = 8;
			_serialPort->Handshake = Windows::Devices::SerialCommunication::SerialHandshake::None;

			// setup our data reader for handling incoming data
			_dataReaderObject = ref new Windows::Storage::Streams::DataReader(_serialPort->InputStream);
			_dataReaderObject->InputStreamOptions = Windows::Storage::Streams::InputStreamOptions::Partial;

			// setup our data writer for handling outgoing data
			_dataWriterObject = ref new Windows::Storage::Streams::DataWriter(_serialPort->OutputStream);

			// Setting this text will trigger the event handler that runs asynchronously for reading data from the input stream

			Listen();
		}
		catch (Platform::Exception ^ex)
		{
			// perform any cleanup needed
			CloseDevice();
		}
	});
}

/// <summary>
/// Returns a task that sends the outgoing data from the sendText textbox to the output stream. 
/// <param name=”jobNum”> current job number to send </param> 
/// <param name=”cancellationToken”> token to cancel transaction </param> 
/// <returns> concurrent task </returns>
/// </summary
Concurrency::task<void> MainPage::WriteAsync(Concurrency::cancellation_token cancellationToken, int jobNum)
{
	_dataWriterObject->WriteByte(0x88);
	vector<int> digits = SeparateIntoDigits(jobNum);
	for (int i = 0; i < 6; i++)
	{
		_dataWriterObject->WriteByte(digits.at(i));
	}
	_dataWriterObject->WriteByte(CreateChecksum(digits));

	Status->Text = "Job number sent!";
	progBar->Value += 30;
	locationTextBlk->Text = "";

	return concurrency::create_task(_dataWriterObject->StoreAsync(), cancellationToken).then([this](unsigned int bytesWritten)
	{
		if (bytesWritten > 0)
		{

		}
	});
}

/// <summary>
/// to push job number digit by digit 
/// <param name=”value”> value to separate </param> 
/// <returns> int vector </returns>
/// </summary>
vector<int> MainPage::SeparateIntoDigits(unsigned int value)
{
	vector<int> digits;
	do
	{
		digits.push_back(value % 10);
	} while (value /= 10);
	reverse(digits.begin(), digits.end());
	return digits;
}

/// <summary>
/// Checksum for validation with tracer
/// <param name=”digits”> int vector of digits </param> 
/// <returns> checksum int </returns>
/// </summary>
int MainPage::CreateChecksum(vector<int> digits)
{
	int sum = 0;
	for (int i = 0; i < 6; i++)
	{
		sum += digits.at(i);
	}
	return sum;
}

/// <summary>
/// Returns a task that reads in the data from the input stream
/// <param name=”cancellationToken”> token to cancel transaction </param> 
/// <returns> concurrent task </returns>
/// </summary
Concurrency::task<void> VEELB::MainPage::ReadAsync(Concurrency::cancellation_token cancellationToken)
{
	unsigned int _readBufferLength = 2048;

	return concurrency::create_task(_dataReaderObject->LoadAsync(_readBufferLength), cancellationToken).then([this](unsigned int bytesRead)
	{
		/*
		Dynamically generate repeating tasks via "recursive" task creation - "recursively" call Listen() at the end of the continuation chain.
		The "recursive" call is not true recursion. It will not accumulate stack since every recursive is made in a new task.
		*/
		Status->Text = "Waiting for location...";
		progBar->Value += 10;
		if (bytesRead > 0)
		{
			//job = consoleEntries[jobCtr - 1].getJob();

			//ToggleEnableMainGridBtns();

			locationTextBlk->Text += _dataReaderObject->ReadString(bytesRead);
			if (locationTextBlk->Text->Length() > 9)
			{
				UpdateLocation();
			}

			// TODO: validate 

			Status->Text = "Location recieved!";
			progBar->Value = 100;
		}
		// start listening again after done with this chunk of incoming data
		Listen();

	});
}

/// <summary>
/// Updates jobs location when location is recieved
/// <returns> void </returns>
/// </summary>
void VEELB::MainPage::UpdateLocation()
{
	Platform::String^ location = locationTextBlk->Text;
	std::wstring str = location->Data();

	std::wstring x = str.substr(3, 3);
	std::wstring y = str.substr(6, 3);

	job->setXPosition(_wtol(x.data()));
	job->setYPosition(_wtol(y.data()));

	consoleEntries[jobCtr-1].setJob(job);
}

/// <summary>
/// Returns the sum of the data bits being sent so a checksum value can be sent to the TRacer for error control
/// <param name=”message”> message </param> 
/// <returns> int </returns>
/// </sumary
int CreateChecksum(Platform::String^ message)
{
	return 5;
}

/// <summary>
/// Initiates task cancellation
/// </summary
void VEELB::MainPage::CancelReadTask(void)
{
	cancellationTokenSource->cancel();
}

/// <summary>
/// Closes the comport currently connected
/// <returns> VOID </returns>
/// </summary
void VEELB::MainPage::CloseDevice(void)
{
	delete(_dataReaderObject);
	_dataReaderObject = nullptr;

	delete(_dataWriterObject);
	_dataWriterObject = nullptr;

	delete(_serialPort);
	_serialPort = nullptr;

}

/// <summary>
/// Event handler that starts listening the serial port input
/// <returns> void </returns>
/// </summary
void VEELB::MainPage::Listen()
{
	try
	{
		if (_serialPort != nullptr)
		{
			// calling task.wait() is not allowed in Windows Runtime STA (Single Threaded Apartment) threads due to blocking the UI.
			concurrency::create_task(ReadAsync(cancellationTokenSource->get_token()));
		}
	}
	catch (Platform::Exception ^ex)
	{
		if (ex->GetType()->FullName == "TaskCanceledException")
		{
			CloseDevice();
		}
		else
		{
		}
	}
}

/// <summary>
/// Device constructor
/// <param name=”id”> identification for device </param> 
/// <param name=”deviceInfo”> device information </param> 
/// <returns> device </returns>
/// </summary>
Device::Device(Platform::String^ id, Windows::Devices::Enumeration::DeviceInformation^ deviceInfo)
{
	_id = id;
	_deviceInformation = deviceInfo;
}

//File Access
/// <summary>
/// creates file on disk depending on type
/// <param name=”fileType”> type of file </param> 
/// <returns> void </returns>
/// </summary>
void MainPage::CreateFile(int fileType)
{
	//null pointer allows for access to the current user
	//Platform::String^ tempFile = fileName;

	switch (fileType)
	{
		case 1:
			concurrency::create_task(KnownFolders::GetFolderForUserAsync(nullptr, KnownFolderId::DocumentsLibrary))
				.then([this](StorageFolder^ documentsFolder)
			{
				Platform::String^ fileName = "configFile.txt";
				return documentsFolder->CreateFileAsync(fileName, CreationCollisionOption::OpenIfExists);
			}).then([this](concurrency::task<StorageFile^> task)
			{
				try
				{
					ConfigFile = task.get();
					// success
				}
				catch (Platform::Exception^ e)
				{
					// I/O errors are reported as exceptions.
					// TODO: notify error
				}
			});
			break;
		case 2:
			concurrency::create_task(KnownFolders::GetFolderForUserAsync(nullptr, KnownFolderId::DocumentsLibrary))
				.then([this](StorageFolder^ documentsFolder)
			{
				Platform::String^ fileName = "Log.txt";
				return documentsFolder->CreateFileAsync(fileName, CreationCollisionOption::OpenIfExists);
			}).then([this](concurrency::task<StorageFile^> task)
			{
				try
				{
					LogFile = task.get();
					// success
				}
				catch (Platform::Exception^ e)
				{
					// I/O errors are reported as exceptions.
					// TODO: notify error
				}
			});
			break;
	}
}

/// <summary>
/// Writes text to file depending on type
/// <param name=”fileType”> type of file </param> 
/// <param name=”inputText”> string to right to the file </param> 
/// <returns> bool </returns>
/// </summary>
bool MainPage::WriteTextToFile(int fileType, Platform::String^ inputText)
{
	if (fileType == 1)
	{
		StorageFile^ tempFile = ConfigFile;
		if (tempFile != nullptr)
		{
			Platform::String^ userContent = inputText;
			if (userContent != nullptr && !userContent->IsEmpty())
			{
				concurrency::create_task(FileIO::WriteTextAsync(tempFile, userContent)).then([this, tempFile, userContent](concurrency::task<void> task)
				{
					try
					{
						task.get();
						return true;
					}
					catch (COMException^ ex)
					{
						// TODO: notify error
						return false;
					}
				});
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}
	else if (fileType == 2)
	{
		StorageFile^ tempFile1 = LogFile;
		if (tempFile1 != nullptr)
		{
			Platform::String^ userContent = inputText;
			if (userContent != nullptr && !userContent->IsEmpty())
			{
				concurrency::create_task(FileIO::WriteTextAsync(tempFile1, userContent)).then([this, tempFile1, userContent](concurrency::task<void> task)
				{
					try
					{
						task.get();
						return true;
					}
					catch (COMException^ ex)
					{
						return false;
					}
				});
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

/// <summary>
/// Reads text from file depending on type
/// <param name=”fileType”> type of file </param> 
/// <returns> void </returns>
/// </summary>
void MainPage::ReadTextFromFile(int fileType)
{
	if (fileType == 1)
	{
		StorageFile^ tempFile = ConfigFile;
		if (tempFile != nullptr)
		{
			concurrency::create_task(FileIO::ReadTextAsync(tempFile)).then([this, tempFile](concurrency::task<Platform::String^> task)
			{
				try
				{
					Platform::String^ fileContent = task.get();
					configTextBlk->Text = fileContent;
				}
				catch (COMException^ ex)
				{
					Platform::String^ fileError = "Error";
				}
			});
		}
		else
		{
		}
	}
	else if (fileType == 2)
	{
		StorageFile^ tempFile = LogFile;
		if (tempFile != nullptr)
		{
			concurrency::create_task(FileIO::ReadTextAsync(tempFile)).then([this, tempFile](concurrency::task<Platform::String^> task)
			{
				try
				{
					Platform::String^ fileContent = task.get();
					Status->Text = fileContent;
				}
				catch (COMException^ ex)
				{
					Platform::String^ fileError = "Error";
				}
			});
		}
		else
		{
		}
	}
	else
	{

	}
}