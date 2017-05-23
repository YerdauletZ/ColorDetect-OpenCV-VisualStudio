/**
* By Yerdaulet Zeinolla
* */

#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <math.h>
#include <vector>

using namespace cv;
using namespace std;

//Assign all global variables
int radius = 100;
bool colorSelected = false, surpriseOn = false, choosePic=false;
Mat frame;
Mat temp;
Mat frame1;
Vec3b bgr;
int swapInfoX[10][10], swapInfoY[10][10];
int counter = 0;
int iX1, iY1, oX1, oY1;
///////////////////

//function which updates the image called by Surprise button
Mat refreshSurprise(const cv::Mat& src, int cw, int ch)
{
	using namespace cv;
	int w(frame.cols);
	int h(frame.rows);
	CV_Assert(w >= cw);
	CV_Assert(h >= ch);
	Mat dst(3 * ch, 4 * cw, frame.type());
	int k = 0; 
	for (int i = 0; i < 4;i++)
		for (int j = 0; j < 3;j++)
		{
			Mat(src, Rect(swapInfoX[i][j]*160, swapInfoY[i][j]*160, cw, ch)).copyTo(Mat(dst, Rect(i*160, j*160, cw, ch)));
			k++;
		}
	return dst;
}

//function Swaps the coordinates of blocks
static void SwapPic(int iX, int iY, int oX, int oY)//iX - iniial X, oX - output X
{
	int rowNi, colNi, rowNf, colNf; //rowNi - initial Row Number, rowNf - final Row Number
	colNi = (iX / 160); rowNi = (iY / 160);
	colNf = (oX / 160); rowNf = (oY / 160);
	int tempx = swapInfoX[colNi][rowNi], tempy = swapInfoY[colNi][rowNi];
	swapInfoX[colNi][rowNi] = swapInfoX[colNf][rowNf];
	swapInfoY[colNi][rowNi] = swapInfoY[colNf][rowNf];
    
	swapInfoX[colNf][rowNf] = tempx;
	swapInfoY[colNf][rowNf] = tempy;
}

//Function for mouse operation in Surprise window
static void surpriseMouse(int event, int x, int y, int flag, void* param){
	if (event == CV_EVENT_LBUTTONDOWN && surpriseOn == true) {

		if (!choosePic) { iX1 = x; iY1 = y; choosePic = true; }

	}
	else if (event == CV_EVENT_RBUTTONDOWN && surpriseOn == true) {
		if (choosePic) { oX1 = x; oY1 = y; choosePic = false; SwapPic(iX1, iY1, oX1, oY1); }
	}
}

//Mouse Function
static void clickMouse(int event, int x, int y, int flag, void* param)
{   
	if (event == CV_EVENT_LBUTTONDOWN) {
		bgr = temp.at<Vec3b>(Point(x, y));
		colorSelected = true;	
	}
	return;
}

//Gives Initial Annotations
static void Guide() {
	cout << "////////////////-------READ ME-------/////////////////" << endl;

	cout << "\t Make a click with the Left Button of a mouse for a Color Selection \n"
		"\t B using the trackbar you can adjust the R for colo enclosure\n"<< endl;

	cout << "\t Use the Hotkeys on a keyboard:\n"
		"\t 'r'	  - switchess On/Off the color selection\n"
		"\t 's'   - Calls and Stops the SURPRISE\n" 
		"\t 'ESC' - Stop The Program\n" << endl;

	cout << "\t In SURPISE! window:\n"
		"\t Mouse Left click	  - Select the First image\n"
		"\t Mouse Right click     - Select the Second image\n"
		"\t 'ESC'                 - Stop The Program\n" << endl;
	cout << "////////////////-------READ ME-------/////////////////" << endl;
	return;
}

int main(int, char**)
{
	Guide();

	int m = 3, n = 2;
	for (int i = 0; i < 4; i++)//iitial adjustments for Surprise
		for (int j = 0; j < 3; j++) {
			
			swapInfoX[m][n] = i;
			swapInfoY[m][n] = j;

			if (m == 3) m = 0; else m++;
			if (n == 2) n = 0; else n++;
		}

	VideoCapture cap(0);
	if (!cap.isOpened())
		return -1;

	namedWindow("Color Detection", 1); 
	setMouseCallback("Color Detection", clickMouse, 0); 
	createTrackbar("Enclosure", "Color Detection", &radius, 255, 0, 0);
	while (1) {
		cap >> frame;
		temp = frame*1.3;
		if (colorSelected) {
			for (int y = 0; y<frame.rows; y++) {
				for (int x = 0; x<frame.cols; x++) {
					Vec3b pixel = temp.at<Vec3b>(Point(x, y));
					if ((pow(pixel[2] - bgr[2], 2) + pow(pixel[1] - bgr[1], 2) + pow(pixel[0] - bgr[0], 2))>pow(radius, 2)) {
						pixel[2] = int((pixel[2] + pixel[1] + pixel[0]) / 3);
						pixel[1] = pixel[2]; pixel[0] = pixel[1];
					}
					pixel /= 1.3;
					frame.at<Vec3b>(Point(x, y)) = pixel;
					
				}
			}
		}

		//Surprise main Function
		if (surpriseOn) {
			namedWindow("SURPRISE!", 1); setMouseCallback("SURPRISE!", surpriseMouse, 0);
			int stepsSize = 160;
			int width = frame.size().width;
			int height = frame.size().height;			
			Mat res = refreshSurprise(frame, 160, 160);

			for (int i = 0; i<height; i += stepsSize)
				line(res, Point(0, i), Point(width, i), Scalar(0, 255, 255));

			for (int i = 0; i<width; i += stepsSize)
				line(res, Point(i, 0), Point(i, height), Scalar(255, 0, 255));
			
			imshow("SURPRISE!", res);
		}
		imshow("Color Detection", frame);
		int c = waitKey(30);

		if ((char)c == 27) break; if ((char)c == 'r') colorSelected = !colorSelected; if ((char)c == 's') surpriseOn = !surpriseOn; 
	}
	return 0;
}
