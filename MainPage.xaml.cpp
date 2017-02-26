//
// MainPage.xaml.cpp
// Implementation of the MainPage class.
// This class perfoms the event handling of user input and the image processing functions.
//

#include "pch.h"
#include "MainPage.xaml.h"
#include "Device.h"
#include "FileAccess.h";
#include "JobViewModel.h"

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

using namespace Windows::Storage;

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
int xPos = 235;
int yPos = 235;
int redSldr = 0;
int greenSldr = 0;
int blueSldr = 0;
JobViewModel^ job;
FileAccess^ consoleFile;
FileAccess^ configFile;
bool onExit = false;
auto itemCollection = ref new Platform::Collections::Vector<Object^>();


MainPage::MainPage()
{
	InitializeComponent();
	/*Windows::ApplicationModel::Email::EmailMessage^ temp = ref new Windows::ApplicationModel::Email::EmailMessage();

	StorageFile^ attachmentFile;
	Windows::Storage::Streams::RandomAccessStreamReference^ stream = Windows::Storage::Streams::RandomAccessStreamReference::CreateFromFile(attachmentFile);*/
	_availableDevices = ref new Platform::Collections::Vector<Platform::Object^>();
	ListAvailablePorts();

	consoleFile = ref new FileAccess("console.txt");
	configFile = ref new FileAccess(L"config.dat");
}


// Webcam functions
void cvVideoTask()
{
	cv::Mat frame, oldFrame, tmp, grayScale, canny, canny_output;

	vector<vector<cv::Point> > contours;
	vector<Vec4i> hierarchy;

	/*xPos = job->getXPosition;
	yPos = job->getYPosition;*/

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

		if (onExit)
		{
			break;
		}

	}

	cam.release();
}


int seuil = 10;
void Compare(Mat frame, Mat oldFrame, Mat grayScale)
{
	Mat canny, gray;
	int colourVal = 0;
	int topX = 0;
	int topY = 0;
	int bottomX = 0;
	int bottomY = 0;
	int first = 0;
	int changed = 0;
	int thresh = 100;

	double blue = (double)blueSldr;
	double green = (double)greenSldr;
	double red = (double)redSldr;

	cv::line(frame, cv::Point(xPos, yPos - 10), cv::Point(xPos, yPos + 10), Scalar(red, green, blue), 3);
	cv::line(frame, cv::Point(xPos - 10, yPos), cv::Point(xPos + 10, yPos), Scalar(red, green, blue), 3);
	cv::circle(frame, cv::Point(xPos, yPos), radius, Scalar(red, green, blue), 10, 8, 0);
	//cv::circle(frame, cv::Point(xPos, yPos), radius, Scalar(0, 204, 0), 10, 8, 0);

	cvtColor(frame, gray, CV_BGR2GRAY); // convert to grayscale
										//blur(gray, gray, cv::Size(3, 3)); // blur converted mat

	Canny(gray, canny, thresh, thresh * 2, 3); // apply canny filter to image
	try
	{
		for (int i = 0; i < frame.rows; i++)
		{
			for (int j = 0; j < frame.cols; j++)
			{
				Vec3b colour = frame.at<Vec3b>(i, j);
				//Scalar colour = gray.at<uchar>(cv::Point(i, j));
				/*if (colour.val[0] == 255 && colour.val[1] == 255 && colour.val[2] == 255)
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
				}*/
				/*if (colour.val[0] == 255)
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
				}*/

				if (colour.val[0] < 100 && colour.val[1] < 100 && colour.val[2] < 100)
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
	}
	catch (Platform::Exception^ e)
	{
		Platform::String^ temp = e->Message;
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

	VideoCapture cap;

	RNG rng(12345);

	try
	{
		cap.open(0);
		cap >> src;
		winrt_imshow();
		Sleep(5000);
		while (1)
		{
			if (!cap.grab()) continue;
			cap >> src;

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
	catch (...)
	{

	}
}


void VEELB::MainPage::exitWebcamBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	onExit = true;

	WebcamFeedGrid->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
	MainGrid->Visibility = Windows::UI::Xaml::Visibility::Visible;

	Platform::String^ txt = redSldr.ToString() + " " + greenSldr.ToString() + " " + blueSldr.ToString();
	if (configFile->WriteTextToFile(txt))
	{
	}
}


// Event handlers
// TODO: Remove
void VEELB::MainPage::Page_Loaded(Object^ sender, RoutedEventArgs^ e)
{
}


void VEELB::MainPage::initBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	/*Status->Text = "Loading config...";
	progBar->Value = 50;
	Platform::String^ temp;
	try
	{
	Platform::String^ config = configFile->ReadTextFromFile();
	}
	catch (Platform::Exception^ e)
	{
	temp = e->Message;
	}
	progBar->Value = 100;*/

	MainGrid->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
	WebcamFeedGrid->Visibility = Windows::UI::Xaml::Visibility::Visible;

	winrt_setFrameContainer(imgCV);
	winrt_startMessageLoop(cvVideoTask);

	onExit = false;
	//CameraFeed();
}


void VEELB::MainPage::screenSaverAnimation()
{
	//rows 768, cols 432
	screenSaverImg->Visibility = Windows::UI::Xaml::Visibility::Visible;
	while (1)
	{
		for (int i = 0; i < 760; i = i + 10)
		{
			for (int j = 0; j < 430; j = j + 2)
			{
				//(left, top, right, bottom);

				screenSaverImg->Margin = Thickness((double)j, (double)i, (double)(432 - j), (double)(768 - 0));
				Sleep(1000);
			}
		}
	}
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
	jobIdNumTxtBlock->Text = "";
	jobNumString = jobIdNumTxtBlock->Text;
}


void MainPage::returnBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	bool isValid = jobIdNumTxtBlock->Text->Length == 6;
	jobNumInt = _wtoi(jobNumString->Data());

	job = ref new JobViewModel(jobNumInt);

	//xPos = job->getXPosition();

	// save last returned number
	mainGridJobNumberTxtBlk->Text = "Job number for session: " + jobNumString;

	JobNumberGrid->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
	MainGrid->Visibility = Windows::UI::Xaml::Visibility::Visible;

	//consoleFile->WriteTextToFile("Job number: " + jobNumString);

	////_serialPort = ref new Windows::Devices::SerialCommunication::SerialDevice::FromIdAsync(entry->Id);

	//TextBlock^ txtBlock = ref new TextBlock();
	//ListBoxItem^ lbItem = ref new ListBoxItem();
	//txtBlock->Text = consoleFile->ReadTextFromFile();

	//lbItem->Content = txtBlock;
	//itemCollection->Append(lbItem);

	// TODO: implement necessary functions in console

	//SerialProgressBar->Value = 25;

	if (_serialPort != nullptr)
	{
		try
		{
			if (isValid)
			{
				WriteAsync(cancellationTokenSource->get_token(), jobNumInt);
			}
			else
			{
				//status->Text = "Enter the text you want to write and then click on 'WRITE'";
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

	concurrency::create_task(ConnectToSerialDeviceAsync(entry, cancellationTokenSource->get_token()));

	SplashScreenGrid->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
	MainGrid->Visibility = Windows::UI::Xaml::Visibility::Visible;

	ProgressGrid->Visibility = Windows::UI::Xaml::Visibility::Visible;
}


void VEELB::MainPage::exitJobNumberBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	JobNumberGrid->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
	MainGrid->Visibility = Windows::UI::Xaml::Visibility::Visible;

	// remove last job
	jobIdNumTxtBlock->Text = "";

	// TODO: retrieve last job 
}


void VEELB::MainPage::sleepBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	MainGrid->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
	ScreenSaverGrid->Visibility = Windows::UI::Xaml::Visibility::Visible;
	screenSaverImg->Visibility = Windows::UI::Xaml::Visibility::Visible;

	screenSaverAnimation();
}


void MainPage::settingsWebcamBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	WebcamSplitter->IsPaneOpen = !WebcamSplitter->IsPaneOpen;
	//SerialProgressBar->Value = 50;
}


/// <summary>
/// Finds all serial devices available on the device and populates a list with the Ids of each device.
/// </summary>
void MainPage::ListAvailablePorts(void)
{
	cancellationTokenSource = new Concurrency::cancellation_token_source();

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


/// <Summary>
/// Determines if the device Id corresponds to the Tracer or another type of serial device since more
/// devices may be connected to the Pi in the future for the company as they add features to their overall
/// system of making lesnes
bool MainPage::IsTracer(Platform::String^ id)
{
	if (id == "\\")
		return true;
	else
		return false;
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
/// Creates a task chain that attempts connect to a serial device asynchronously. 
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

	_dataWriterObject->WriteString(jobIdNumTxtBlock->Text);

	return concurrency::create_task(_dataWriterObject->StoreAsync(), cancellationToken).then([this](unsigned int bytesWritten)
	{
		if (bytesWritten > 0)
		{
			Status->Text += "bytes written successfully!";
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
		if (bytesRead > 0)
		{
			Status->Text = _dataReaderObject->ReadString(bytesRead);
			/*byte temp;
			for (int i = 0; i < bytesRead; i++)
			{
				job->setData(_dataReaderObject->ReadByte());
			}*/
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
	cancellationTokenSource->cancel();
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
/// Event handler that starts listening the serial port input
/// </summary
void VEELB::MainPage::Listen()
{
	try
	{
		if (_serialPort != nullptr)
		{
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


int main()
{
}