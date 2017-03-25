/// MainPage.xaml.cpp
/// <summary>
/// Implementation of the MainPage class.
/// This class perfoms the event handling of user input, the image processing functions. TODO: Move image processing to own file?
/// </summary>
/// <author> Brayden Folk </author>
/// <author> Petra Kujawa </author>


#include "pch.h"
#include "MainPage.xaml.h"
#include "Console.h"
#include "Device.h"
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

VideoCapture cam;
int xPos = 235;
int yPos = 235;
int redSldr = 0;
int greenSldr = 0;
int blueSldr = 0;
int thicknessSldr = 0;

JobViewModel^ job;
//std::vector<Console> consoleEnt;


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

	consoleCtr = 0;
}

// Webcam functions
/// <summary>
/// Background message loop task for webcam feed, allows for functionality such as initializing the webcam,
///		retrieving frames from the webcam and passes frames to compare which is where the main work is being done.
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
void Compare(Mat frame, Mat oldFrame, Mat grayScale)
{
	Mat canny, gray;
	int colourVal = 0;
	int topX = 0;
	int topY = 0;
	int bottomX = 0;
	int bottomY = 0;
	int leftY = 0;
	//int topX = 0;
	int rightY = 0;
	//int bottomY = 0;
	int first = 0;
	int changed = 0;
	int thresh = 100;

	double blue = (double)blueSldr;
	double green = (double)greenSldr;
	double red = (double)redSldr;
	double thickness = (double)thicknessSldr;

	int redLocked = 255;
	int greenLocked = 0;
	int blueLocked = 0;


	int compareCtr = 0;
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

	bool motion = false;

	if (compareCtr > 1000 /*|| abs(leftY - rightY) < 50*/)
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
							/*topX = i;
							topY = j;*/
							topX = i;
							leftY = j;

							/*int clusterCtr = 0;
							for (int p = 0; p < 7; p++)
							{
							Vec3b clusterColour = frame.at<Vec3b>(i, j + p);
							if (clusterColour.val[0] < 60 && clusterColour.val[1] < 60 && clusterColour.val[2] < 60)
							{
							clusterCtr++;
							if (clusterCtr > 15)
							{
							break;
							}
							}
							}*/

							/*if (clusterCtr > 5)
							{*/
							first = 1;

							//}
						}

						/*bottomX = i;
						bottomY = j;*/
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
		// Template line
		cv::line(frame, cv::Point(0, 0), cv::Point(frame.cols, 0), Scalar(red, green, blue), thickness);

		int midX = (topX + bottomX) / 2;
		int midY = (leftY + rightY) / 2;

		cv::Point pt1(leftY, topX + 5);
		cv::Point pt2(rightY, bottomX - 5);

		cv::Point dv(0, 0); // get direction vector
		dv.x = pt2.x - pt1.x;
		dv.y = pt2.y - pt1.y;

		cv::line(frame, cv::Point(midY, midX + 25), cv::Point(midY, midX - 25), Scalar(red, green, blue), thickness);
		cv::line(frame, pt1, pt2, Scalar(red, green, blue), thickness);
		cv::circle(frame, cv::Point(midY, midX), thickness, Scalar(150, 255, 0), 4, 8, 0);

		if (abs(midY - xPos) < 5 && abs(midX - yPos) < 5)
		{
			redLocked = 0;
			greenLocked = 255;
			blueLocked = 0;
		}
	}

	cv::line(frame, cv::Point(xPos, yPos - 25), cv::Point(xPos, yPos + 25), Scalar(redLocked, greenLocked, blueLocked), thickness);
	cv::line(frame, cv::Point(xPos - 55, yPos), cv::Point(xPos + 55, yPos), Scalar(redLocked, greenLocked, blueLocked), thickness);
}

// UI Functions
void VEELB::MainPage::exitWebcamBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	onExit = true;

	WebcamFeedGrid->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
	MainGrid->Visibility = Windows::UI::Xaml::Visibility::Visible;

	Platform::String^ txt = redSldr.ToString() + " " + greenSldr.ToString() + " " + blueSldr.ToString() + " " + thicknessSldr.ToString();

	if (WriteTextToFile(1, txt))
	{
		// success
	}
}

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
void VEELB::MainPage::initBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	Platform::String^ temp;
	try
	{
		/*locationTextBlk->Text == "";
		if (locationTextBlk->Text == "")
		{
		CustomMessageDialog("Location has not yet been recieved, continue?");
		return;
		}*/
		if (jobNumInt > 0)
		{
			Platform::String^ location = locationTextBlk->Text;
			std::wstring str = location->Data();

			std::wstring x = str.substr(3, 3);
			std::wstring y = str.substr(6, 3);

			xPos = _wtol(x.data());
			yPos = _wtol(y.data());
		}
		else
		{
			xPos = 0;
			yPos = 0;
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

void VEELB::MainPage::screenSaverAnimation()
{
	//rows 768, cols 432
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


void VEELB::MainPage::enterJobNumberBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	MainGrid->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
	JobNumberGrid->Visibility = Windows::UI::Xaml::Visibility::Visible;
}


void VEELB::MainPage::validate(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	if (jobIdNumTxtBlock->Text->Length() == 6)
	{
		backspaceBtn_Click(sender, e);
	}
}


void VEELB::MainPage::oneBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	validate(sender, e);
	jobIdNumTxtBlock->Text += "1";
	_jobNumString = jobIdNumTxtBlock->Text;
}


void VEELB::MainPage::twoBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	validate(sender, e);
	jobIdNumTxtBlock->Text += "2";
	_jobNumString = jobIdNumTxtBlock->Text;
}


void VEELB::MainPage::threeBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	validate(sender, e);
	jobIdNumTxtBlock->Text += "3";
	_jobNumString = jobIdNumTxtBlock->Text;
}


void VEELB::MainPage::fourBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	validate(sender, e);
	jobIdNumTxtBlock->Text += "4";
	_jobNumString = jobIdNumTxtBlock->Text;
}


void VEELB::MainPage::fiveBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	validate(sender, e);
	jobIdNumTxtBlock->Text += "5";
	_jobNumString = jobIdNumTxtBlock->Text;
}


void VEELB::MainPage::sixBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	validate(sender, e);
	jobIdNumTxtBlock->Text += "6";
	_jobNumString = jobIdNumTxtBlock->Text;
}


void VEELB::MainPage::sevenBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	validate(sender, e);
	jobIdNumTxtBlock->Text += "7";
	_jobNumString = jobIdNumTxtBlock->Text;
}


void VEELB::MainPage::eightBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	validate(sender, e);
	jobIdNumTxtBlock->Text += "8";
	_jobNumString = jobIdNumTxtBlock->Text;
}


void VEELB::MainPage::nineBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	validate(sender, e);
	jobIdNumTxtBlock->Text += "9";
	_jobNumString = jobIdNumTxtBlock->Text;
}


void VEELB::MainPage::zeroBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	validate(sender, e);
	jobIdNumTxtBlock->Text += "0";
	_jobNumString = jobIdNumTxtBlock->Text;
}


void VEELB::MainPage::backspaceBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	Platform::String^ number = jobIdNumTxtBlock->Text;

	if (number->Length() > 0)
	{
		string jobIdStdString = ConvertPlatformStringToStandardString(number);
		jobIdNumTxtBlock->Text = ConvertStringToPlatformString(jobIdStdString);
		_jobNumString = jobIdNumTxtBlock->Text;
	}
}


string VEELB::MainPage::ConvertPlatformStringToStandardString(Platform::String^ inputString)
{
	wstring tempw(inputString->Begin());
	string jobIdStdString(tempw.begin(), tempw.end() - 1);
	return jobIdStdString;
}


Platform::String^ VEELB::MainPage::ConvertStringToPlatformString(string inputString)
{
	wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	wstring intermediateForm = converter.from_bytes(inputString);
	Platform::String^ retval = ref new Platform::String(intermediateForm.c_str());
	return retval;
}


void VEELB::MainPage::clearBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	jobIdNumTxtBlock->Text = "";
	_jobNumString = jobIdNumTxtBlock->Text;
}


void MainPage::returnBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	if (jobIdNumTxtBlock->Text->Length() != 6)
	{
		//jobIdNumTxtBlock->Foreground = std::Scalar(255, 0, 0);
		// TODO: change foregroud colour, notify user that it must be 6 digits, set error flag so that we can notify the number button clicks
		return;
	}

	jobNumInt = _wtoi(_jobNumString->Data());

	job = ref new JobViewModel(jobNumInt);
	
	// save last returned number
	mainGridJobNumberTxtBlk->Text = "Job number for session: " + _jobNumString;

	JobNumberGrid->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
	MainGrid->Visibility = Windows::UI::Xaml::Visibility::Visible;

	TextBlock^ txtBlock = ref new TextBlock();
	ListBoxItem^ lbItem = ref new ListBoxItem();

	Windows::Globalization::Calendar^ c = ref new Windows::Globalization::Calendar;
	Platform::String^ dateTimeNow = c->Year.ToString() + "-" + c->Month + "-" + c->Day + " " + c->Hour + ":" + c->Minute + ":" + c->Second;

	txtBlock->Text = dateTimeNow + "        Job: " + _jobNumString;

	lbItem->Content = txtBlock;
	itemCollection->Append(lbItem);
	ConsoleListBox->ItemsSource = itemCollection;

	//int ctr = currentConsole->getJobCtr();
	//consoleEntries


	// TODO: implement necessary functions in console

	if (_serialPort != nullptr)
	{
		try
		{
			if (jobIdNumTxtBlock->Text->Length() > 0)
			{
				Status->Text = "Writing job number...";
				progBar->Value = 10;
				WriteAsync(_cancellationTokenSource->get_token(), jobNumInt);
			}
			else
			{
			}
		}
		catch (Platform::Exception ^ex)
		{
			//status->Text = "sendTextButton_Click: " + ex->Message;
		}
	}
}


void VEELB::MainPage::ScreenSaverGrid_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
{
	ScreenSaverGrid->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
	MainGrid->Visibility = Windows::UI::Xaml::Visibility::Visible;
}


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


void VEELB::MainPage::startBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	Device^ selectedDevice = static_cast<Device^>(_availableDevices->GetAt(0));
	Windows::Devices::Enumeration::DeviceInformation ^entry = selectedDevice->DeviceInfo;

	concurrency::create_task(ConnectToSerialDeviceAsync(entry, _cancellationTokenSource->get_token()));

	SplashScreenGrid->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
	MainGrid->Visibility = Windows::UI::Xaml::Visibility::Visible;

	ProgressGrid->Visibility = Windows::UI::Xaml::Visibility::Visible;

	ReadTextFromFile(1);
}


void VEELB::MainPage::exitJobNumberBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	JobNumberGrid->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
	MainGrid->Visibility = Windows::UI::Xaml::Visibility::Visible;

	// TODO: retrieve last job 
}


void VEELB::MainPage::sleepBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	MainGrid->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
	ScreenSaverGrid->Visibility = Windows::UI::Xaml::Visibility::Visible;
	screenSaverImg->Visibility = Windows::UI::Xaml::Visibility::Visible;

	screenSaverAnimation();
}


// Settings flyout 
void MainPage::settingsWebcamBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	WebcamSplitter->IsPaneOpen = !WebcamSplitter->IsPaneOpen;
}

void VEELB::MainPage::redSlider_ValueChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs^ e)
{
	redSldr = redSlider->Value;
}

void VEELB::MainPage::greenSlider_ValueChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs^ e)
{
	greenSldr = greenSlider->Value;
}

void VEELB::MainPage::blueSlider_ValueChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs^ e)
{
	blueSldr = blueSlider->Value;
}

void VEELB::MainPage::thicknessSlider_ValueChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs^ e)
{
	thicknessSldr = thicknessSlider->Value;
}


/*****************************************  Serial Communication  ****************************************/

/// <summary>
/// Finds all serial devices available on the device and populates a list with the Ids of each device.
/// </summary>
void MainPage::ListAvailablePorts(void)
{
	_cancellationTokenSource = new Concurrency::cancellation_token_source();
	// Using asynchronous operation, get a list of serial devices available on this device

	Concurrency::create_task(ListAvailableSerialDevicesAsync()).then([this](Windows::Devices::Enumeration::DeviceInformationCollection ^serialDeviceCollection)
	{
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
/// </summary>
Windows::Foundation::IAsyncOperation<Windows::Devices::Enumeration::DeviceInformationCollection ^> ^MainPage::ListAvailableSerialDevicesAsync(void)
{
	// Construct AQS String for all serial devices on system
	Platform::String ^serialDevices_aqs = Windows::Devices::SerialCommunication::SerialDevice::GetDeviceSelector();

	// Identify all paired devices satisfying query
	return Windows::Devices::Enumeration::DeviceInformation::FindAllAsync(serialDevices_aqs);
}


/// <summary>
/// Creates a task chain that attempts to connect to a serial device asynchronously. 
/// </summary
Concurrency::task<void> MainPage::ConnectToSerialDeviceAsync(Windows::Devices::Enumeration::DeviceInformation ^device1, Concurrency::cancellation_token cancellationToken)
{
	return Concurrency::create_task(Windows::Devices::SerialCommunication::SerialDevice::FromIdAsync(device1->Id), cancellationToken)
		.then([this](Windows::Devices::SerialCommunication::SerialDevice ^serial_device)
	{
		try
		{
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
/// Returns a task that sends the outgoing data to the output stream. 
/// <param name = cancellationToken> a cancellation token for creating tasks </param>
/// <param name="jobNum"> The job number to be sent to the Tracer </param>
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
	
	return concurrency::create_task(_dataWriterObject->StoreAsync(), cancellationToken).then([this](unsigned int bytesWritten)
	{
		if (bytesWritten > 0)
		{
			Status->Text = "Job number sent!";
			progBar->Value += 30;
			locationTextBlk->Text = "";
		}
		jobIdNumTxtBlock->Text = "";
	});
}


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
/// Returns the sum of the data bits being sent so a checksum value can be sent to the Tracer for error control
/// </sumary
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
			locationTextBlk->Text += _dataReaderObject->ReadString(bytesRead);
			// TODO: validate 
			Status->Text = "Location obtained!";
			progBar->Value = 100;
		}
		// start listening again after done with this chunk of incoming data to get future job numbers
		Listen();
	});
}


/// <summary>
/// Initiates task cancellation
/// </summary
void VEELB::MainPage::CancelReadTask(void)
{
	_cancellationTokenSource->cancel();
}


/// <summary>
/// Closes the comport currently connected
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
/// Event handler that starts listening to the serial port input
/// </summary
void VEELB::MainPage::Listen()
{
	try
	{
		if (_serialPort != nullptr)
		{
			concurrency::create_task(ReadAsync(_cancellationTokenSource->get_token()));
		}
	}
	catch (Platform::Exception ^ex)
	{
		if (ex->GetType()->FullName == "TaskCanceledException")
		{
			CloseDevice();
		}
	}
}


/**************************  File Access  **************************/

void MainPage::CreateFile(int fileType)
{
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
			//// TODO: notify file does not exist error
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
			//// TODO: notify file does not exist error
			return false;
		}
	}
	else
	{
		return false;
	}
}


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
					//return fileContent;
				}
				catch (COMException^ ex)
				{
					// TODO: notify error
					Platform::String^ fileError = "Error";
					//return fileError;
				}
			});
		}
		else
		{
			// TODO: notify file does not exist error
			//return "Error";
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
					//return fileContent;
				}
				catch (COMException^ ex)
				{
					// TODO: notify error
					Platform::String^ fileError = "Error";
					//return fileError;
				}
			});
		}
		else
		{
			// TODO: notify file does not exist error
			//return "Error";
		}
	}
	else
	{

	}
}


void VEELB::MainPage::settingsBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	initBtn_Click(sender, e);
	WebcamSplitter->IsPaneOpen = true;
}


int main()
{
}