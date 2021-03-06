#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;

int is_white(Mat frame, int row, int col);

int main(int, char**)
{
    VideoCapture cap(0); // open the default camera

    Mat edges;
    Mat stream_left ( 360 , 180, CV_8UC1 );
    Mat stream_right (360, 180, CV_8UC1 );
    namedWindow("left",1);
    namedWindow("right",2);

    while (1)  {

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
        Rect roi2 = Rect( 0, 0, width/3, height );
        Mat tmp = view.clone();
        tmp(roi2).copyTo(stream_left);
        Rect roi2_2 = Rect(360,0,180,360);
        tmp(roi2_2).copyTo(stream_right);

        int left_pixel = 0;
        int right_pixel = 0;

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

        if ( left_pixel >= 10 )
            std::cout <<  "Turn Left" << std::endl;
        else if ( right_pixel >= 10 )
            std::cout <<  "Turn Right" << std::endl;
        else std::cout << "Forward" << std::endl;

        imshow("left", stream_left);
        imshow("right", stream_right);

        if(waitKey(30) >= 0) break;
    }
    // the camera will be deinitialized automatically in VideoCapture destructor
    return 0;
}

int is_white(Mat frame, int row, int col) {
    return frame.at<uchar>(row, col) == 1;
}



