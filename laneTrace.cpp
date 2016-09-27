#include "opencv2/opencv.hpp"
#include <time.h>

using namespace cv;
using namespace std;

void sleep( unsigned int mseconds );

int main(int, char**)
{
    VideoCapture cap(0); // open the default camera

    Mat edges;
    Mat stream_left ( 360 , 180, CV_8UC1 );
    Mat stream_right (360, 180, CV_8UC1 );
    namedWindow("left",1);
    namedWindow("right",2);

    FILE *file;

    while ( waitKey(1) != 'q' )  {

        file = fopen("/dev/ttyACM0","w");
        Mat frame;
        cap >> frame; // get a new frame from camera
        cvtColor(frame, edges, COLOR_BGR2GRAY);
        GaussianBlur(edges, edges, Size(7,7), 1.5, 1.5);
        Canny(edges, edges, 0, 30, 3);

        int width = edges.cols;
        int height = edges.rows;

        //left side
        Point L1 = Point(0, 0);
        Point L2 = Point(width/3, 0);
        Point L3 = Point(width/6, height);
        Point L4 = Point(0, height);
        vector<Point> point_L;

        point_L.push_back(L1);
        point_L.push_back(L2);
        point_L.push_back(L3);
        point_L.push_back(L4);

        //right side
        Point R1 = Point(width/3*2,0);
        Point R2 = Point(width,0);
        Point R3 = Point(width,height);
        Point R4 = Point(width/6*5,height);
        vector<Point> point_R;

        point_R.push_back(R1);
        point_R.push_back(R2);
        point_R.push_back(R3);
        point_R.push_back(R4);

        //fill screen with black and draw ROI in white
        Mat roi1(height, width, CV_8U, Scalar(0));
        fillConvexPoly(roi1, point_L , Scalar(255));
        fillConvexPoly(roi1, point_R , Scalar(255));

        Mat view;
        edges.copyTo(view,roi1);

        //ROI_SETTING
        Rect leftRoi = Rect( 0, 0, width/3, height );
        Mat tmp = view.clone();

       //ROI_SETTING
        Rect leftRoi = Rect( 0, 0, width/3, height );
        Mat tmp = view.clone();
        tmp(leftRoi).copyTo(stream_left);

        Rect rightRoi = Rect(360,0,180,360);
        tmp(rightRoi).copyTo(stream_right);

        int left_white = 0;
        int right_white = 0;

        for ( int row = 0; row < stream_left.rows; row++) {
            for ( int col = 0; col < stream_left.cols; col++) {
                if ( stream_left.at<uchar>(row,col) ) {
                    left_white++;
                }
            }
        }

        for ( int row = 0; row < stream_right.rows; row++) {
            for ( int col = 0; col < stream_right.cols; col++) {
                if ( stream_right.at<uchar>(row,col) ) {
                    right_white++;
                }
            }
        }

       if ( left_white >= 50 && right_white < 50 ) {
            fprintf(file, "%c", 'L'); sleep(10); }
        else if ( left_white < 50 && right_white >= 50 ) {
            fprintf(file, "%c", 'R'); sleep(10); }
        else if (left_white < 50 && right_white < 50 ) {
            fprintf(file, "%c", 'F'); sleep(10); }
        else {
            fprintf(file, "%c", 'E'); sleep(10); }

        fclose(file);

        imshow("left", stream_left);
        imshow("right", stream_right);

    }
    // the camera will be deinitialized automatically 
    return 0;

}

void sleep ( unsigned int mseconds ) {

    clock_t goal = mseconds + clock();
    while ( goal > clock());

}

