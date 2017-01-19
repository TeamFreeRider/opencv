//
// Created by Seryung Lee on 9/21/16.
//

#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;

int main(int, char**) {
    VideoCapture cap(0); // open the default camera

    Mat stream_left(360, 180, CV_8UC1);
    Mat stream_right(360, 180, CV_8UC1);
    namedWindow("left", 1);
    namedWindow("right", 2);

    while (1) {

        Mat frame;
        cap >> frame; // get a new frame from camera

        int width = frame.cols;
        int height = frame.rows;

        //left side
        Point L1 = Point(0, 0);
        Point L2 = Point(width / 3, 0);
        Point L3 = Point(width / 6, height);
        Point L4 = Point(0, height);
        vector <Point> left_P;
        left_P.push_back(L1);
        left_P.push_back(L2);
        left_P.push_back(L3);
        left_P.push_back(L4);
        
        //right side
        Point R1 = Point(width / 3 * 2, 0);
        Point R2 = Point(width, 0);
        Point R3 = Point(width, height);
        Point R4 = Point(width / 6 * 5, height);
        vector <Point> right_P;
        right_P.push_back(R1);
        right_P.push_back(R2);
        right_P.push_back(R3);
        right_P.push_back(R4);

        //fill screen with black and draw ROI in white
        Mat roi1(height, width, CV_8U, Scalar(0));

        fillConvexPoly(roi1, left_P, Scalar(255));
        fillConvexPoly(roi1, right_P, Sclar(255));

        Mat view;
        frame.copyTo(view, roi1);

        Rect roi2 = Rect(0, 0, width / 3, height);
        Mat tmp = view.clone();
        tmp(roi2).copyTo(stream_left);

        Rect roi2_2 = Rect(360, 0, 180, 360);
        tmp(roi2_2).copyTo(stream_right);

        /left screen
        Mat left = stream_left.clone();
        Mat right = stream_right.clone();

        cvtColor(stream_left, left, COLOR_BGR2GRAY);
        GaussianBlur(left, left, Size(7, 7), 1.5, 1.5);
        Canny(left, left, 0, 30, 3);

        cvtColor(stream_right, right, COLOR_BGR2GRAY);
        GaussianBlur(right, right, Size(7, 7), 1.5, 1.5);
        Canny(right, right, 0, 30, 3);


        imshow("left", left);
        imshow("right", right);

        if (waitKey(30) >= 0) break;
    }
    // the camera will be deinitialized automatically in VideoCapture destructor
    return 0;
}