
#include <sstream>
#include <string>
#include <iostream>
#include <stdio.h>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/core/core.hpp"

using namespace cv;
using namespace std;

//default capture width and height
int FRAME_WIDTH = 320; //640
int FRAME_HEIGHT = 240; //480

int MIN_OBJECT_AREA = 20*20;

int iLowH = 0;
int iHighH = 179;

int iLowS = 0; 
int iHighS = 255;

int iLowV = 0;
int iHighV = 255;

int centerX, centerY;

string intToString(int number){


	std::stringstream ss;
	ss << number;
	return ss.str();
}

int main( int argc, char** argv ){

	//open serial
	FILE* serial = fopen("/dev/ttyACM0", "w");
	if (serial == 0) {
		printf("Failed to open serial port\n");
	}

	//capture the video from web cam
	VideoCapture cap(0);
 
	// if not success, exit program
	if ( !cap.isOpened() ){  
        	cout << "Cannot open the web cam" << endl;
        	return -1;
	}

	//set height and width of capture frame
	cap.set(CV_CAP_PROP_FRAME_WIDTH,FRAME_WIDTH);
	cap.set(CV_CAP_PROP_FRAME_HEIGHT,FRAME_HEIGHT);

	//create a window called "Control"
	namedWindow("Control", CV_WINDOW_AUTOSIZE);

	//Create trackbars in "Control" window

	cvCreateTrackbar("LowH", "Control", &iLowH, 179); //Hue (0 - 179)
	cvCreateTrackbar("HighH", "Control", &iHighH, 179);

	cvCreateTrackbar("LowS", "Control", &iLowS, 255); //Saturation (0 - 255)
 	cvCreateTrackbar("HighS", "Control", &iHighS, 255);

 	cvCreateTrackbar("LowV", "Control", &iLowV, 255); //Value (0 - 255)
 	cvCreateTrackbar("HighV", "Control", &iHighV, 255);


    	while (true){

        	Mat imgOriginal;

        	bool bSuccess = cap.read(imgOriginal); // read a new frame from video

         	if (!bSuccess){ //if not success, break loop
             		cout << "Cannot read a frame from video stream" << endl;
             		break;
        	}

		//Convert the captured frame from BGR to HSV
  		Mat imgHSV;
  		cvtColor(imgOriginal, imgHSV, COLOR_BGR2HSV); 

		//find center point
		centerX = FRAME_WIDTH/2;
		centerY = FRAME_HEIGHT/2;
		line(imgOriginal,Point(centerX,0),Point(centerX, FRAME_HEIGHT),Scalar(0,255,0),2);
		line(imgOriginal,Point(0,centerY),Point(FRAME_WIDTH, centerY),Scalar(0,255,0),2);
 
		//Threshold the image
		Mat imgThresholded;
		Mat imgContour;

  		inRange(imgHSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgThresholded);
      		inRange(imgHSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgContour);

  		//morphological opening (remove small objects from the foreground)
  		erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
  		dilate( imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );

		erode(imgContour, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
  		dilate( imgContour, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) ); 

  		//morphological closing (fill small holes in the foreground)
  		dilate( imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) ); 
  		erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );

		dilate( imgContour, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) ); 
  		erode(imgContour, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );

		//these two vectors needed for output of findContours
		vector< vector<Point> > contours;
		vector<Vec4i> hierarchy;

		//find contours of filtered image using openCV findContours function
		
		findContours(imgContour,contours,hierarchy,CV_RETR_CCOMP,CV_CHAIN_APPROX_SIMPLE );
		
		//use moments method to find our filtered object
		double refArea = 0;
		if (hierarchy.size() > 0) {
			int numObjects = hierarchy.size();
			for (int index = 0; index >= 0; index = hierarchy[index][0]) {
				Moments moment = moments((cv::Mat)contours[index]);
				double area = moment.m00;
				if(area>MIN_OBJECT_AREA){
					double x = moment.m10/area;
					double y = moment.m01/area;
					//refArea = area;
					circle(imgOriginal,Point(x,y),2,Scalar(0,255,0),2);
					//line(imgOriginal,Point(x,y),Point(x,0),Scalar(0,255,0),2);
					//line(imgOriginal,Point(x,y),Point(x, FRAME_HEIGHT),Scalar(0,255,0),2);
					//line(imgOriginal,Point(x,y),Point(0,y),Scalar(0,255,0),2);
					//line(imgOriginal,Point(x,y),Point(FRAME_WIDTH,y),Scalar(0,255,0),2);
					putText(imgOriginal, intToString(x) + "," + intToString(y), Point(x,y+20), FONT_HERSHEY_COMPLEX, 0.5, Scalar(0, 255, 0), 1, 8);
					if (serial != 0) {
						
      						fprintf(serial, "x%f\n", x);
						printf("SEND x%f\n", x);

      						fprintf(serial, "y%f\n", y);
						printf("SEND y%f\n", y);
    					}	
				}//end if
			}//end for
		}//end if
		
		//imshow("Contour", imgContour);

		//show the thresholded image
  		imshow("Thresholded Image", imgThresholded);

		//show the original image 
  		imshow("Original", imgOriginal);

        	if (waitKey(30) == 27) {//wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
            		cout << "esc key is pressed by user" << endl;
            		break; 
       		}
		
		//waitKey(1);
		
    	} //end while
	
   	return 0;

}
