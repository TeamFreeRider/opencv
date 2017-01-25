
//written in Jan, 19, 2017

//new lane Tracking image processing code




#include "opencv2/opencv.hpp"
#include <time.h>


using namespace cv;
using namespace std;



int is_white(Mat frame, int row, int col);
void sleep( unsigned int mseconds );


int main(int, char**)

{

    VideoCapture cap(0); // open the default camera

    FILE *file;
//    file = fopen("/dev/ttyUSB0", "w");

    Mat stream;
    Mat frame( 100, 150, CV_8UC1 );

    int width = frame.cols;
    int height = frame.rows;

    Mat stream_left ( height/5*3, width/2, CV_8UC1 );
    Mat stream_right ( height/5*3, width/2, CV_8UC1 );

    namedWindow("left",1);
    namedWindow("right",2);
    namedWindow("stream",3);



    while (1)  {
        file = fopen("/dev/ttyUSB0", "w");

        Mat stream;

        cap >> stream; // get a new frame from camera

        resize( stream, frame, Size( 150, 100 ), 0, 0);

        cvtColor(frame, frame, COLOR_BGR2GRAY);
        threshold(frame, frame, 180, 255, THRESH_BINARY);

        Rect leftRect = Rect( 0, height/5*2, width/2, height/5*3 );
        stream_left = frame(leftRect);

        Rect rightRect = Rect(width/2, height/5*2, width/2, height/5*3);
        stream_right = frame(rightRect);

        int left_pixel = 0;
        int right_pixel = 0;


        for ( int row = 0; row < stream_left.rows; row++) {
            for ( int col = 0; col < stream_left.cols; col++) {
                if ( is_white(stream_left, row, col)) {
                    left_pixel++;
                }
            }
        }


        for ( int row = 0; row < stream_right.rows; row++) {
            for ( int col = 0; col < stream_right.cols; col++) {
                if ( is_white(stream_right, row, col)) {
                    right_pixel++;
                }
            }
        }

        int average = (left_pixel + right_pixel)/2;

//        file = fopen("/dev/ttyUSB1", "w");

        if ( average - 70 > left_pixel ){
            std::cout << "L" << std::endl;
            fprintf(file, "%c", 'l');
        }
        else if ( average - 70 > right_pixel ){
            fprintf(file, "%c", 'r');
            std::cout << "R" << std::endl;
        }
        else {
            fprintf(file, "%c", 'f');
            std::cout << "F" << std::endl;
        }

        fclose(file);
	sleep(10);
        std::cout <<  "Left : " << left_pixel << ", Right :" << right_pixel << std::endl;

        imshow("left", stream_left);
        imshow("right", stream_right);

        imshow("stream", stream);

        if(waitKey(30) >= 0) break;

    }

    // the camera will be deinitialized automatically in VideoCapture destructor

//    fclose(file);
    return 0;

}



int is_white(Mat frame, int row, int col) {
    return frame.at<uchar>(row, col) == 255;

}


void sleep ( unsigned int mseconds ) {
    clock_t goal = mseconds + clock();
    while ( goal > clock());

}


