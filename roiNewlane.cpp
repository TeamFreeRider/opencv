//written in Jan, 19, 2017

//new lane Tracking image processing code





#include "opencv2/opencv.hpp"



using namespace cv;

using namespace std;



int is_white(Mat frame, int row, int col);



int main(int, char**)

{

    VideoCapture cap(0); // open the default camera



    Mat stream, frame;

    Mat stream_left ( 360 , 270, CV_8UC1 );

    Mat stream_right (360, 270, CV_8UC1 );

    namedWindow("left",1);

    namedWindow("right",2);

    namedWindow("stream",3);



    while (1)  {



        Mat stream;

        cap >> stream; // get a new frame from camera

        cvtColor(stream, frame, COLOR_BGR2GRAY);

        GaussianBlur(frame, frame, Size(7,7), 1.5, 1.5);

        threshold(frame, frame, 180, 255, THRESH_BINARY);

        int width = frame.cols;

        int height = frame.rows;



        //ROI_SETTING

        Rect leftRect = Rect( 0, 0, width/2, height );

        stream_left = frame(leftRect);

        

        Rect rightRect = Rect(width/2,0,width/2,height);

        stream_right = frame(rightRect);



        for ( int row = 0; row < stream_left.rows; row++) {

            for ( int col = 0; col < stream_left.cols; col++) {

                if ( is_white(stream_left, row, col)) {

                    left_pixel++;

                    if ( left_pixel >= 10 ) break;

                }

            }

        }



        for ( int row = 0; row < stream_right.rows; row++) {

            for ( int col = 0; col < stream_right.cols; col++) {

                if ( is_white(stream_right, row, col)) {

                    right_pixel++;

                    if ( right_pixel >= 10 ) break;

                }

            }

        }


        std::cout <<  "Left : " << left_pixel << ", Right :" << right_pixel << $



        imshow("left", stream_left);

        imshow("right", stream_right);

        imshow("stream", stream);



        if(waitKey(30) >= 0) break;

    }

    // the camera will be deinitialized automatically in VideoCapture destructor

    return 0;

}



int is_white(Mat frame, int row, int col) {

    return frame.at<uchar>(row, col) == 255;
}





