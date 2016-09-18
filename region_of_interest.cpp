#include "opencv2/opencv.hpp"
#include <cv.h>

using namespace cv;
using namespace std;

int main(int, char**)
{
    VideoCapture cap(0); // open the default camera
    if(!cap.isOpened())  // check if we succeeded
        return -1;

    Mat edges;
   // Mat stream(360,200,CV_8UC1);
    namedWindow("edges",1);
    for(;;)
    {
        Mat frame;
        cap >> frame; // get a new frame from camera
        cvtColor(frame, edges, COLOR_BGR2GRAY);
        GaussianBlur(edges, edges, Size(7,7), 1.5, 1.5);
        Canny(edges, edges, 0, 30, 3);

        int width = edges.cols;
        int height = edges.rows;

        //set your roi points : Left
        Point L1 = Point(0, 0);
        Point L2 = Point(width/3*2, 0);
        Piont L3 = Piont(width/2, height);
        Piont L4 = Point(0, height);
        vector<Point> point_L;

        point_L.push_back(L1);
        point_L.push_back(L2);
        point_L.push_back(L3);
        point_L.push_back(L4);

        //Right
        Point R1 = Point(0, 0);
        Point R2 = Point(width/3*2, 0);
        Piont R3 = Piont(width/2, height);
        Piont R4 = Point(0, height);
        vector<Point> point_R;

        point_R.push_back(R1);
        point_R.push_back(R2);
        point_R.push_back(R3);
        point_R.push_back(R4);


        convexHull(points,points2); // to assure correct point order
        Mat roi(height, width, CV_8U, Scalar(0)); //black image
        fillConvexPoly(roi, point_L, Scalar(255)); // draw ROI in white

        //Filter original iamge according to ROI
        Mat filtered;
        edges.copyTo(filtered,roi);

/*
        //ROI_SETTING
        Rect roi = Rect( 0,0,100,360 );
        Mat tmp = edges.clone();
        tmp(roi).copyTo(stream(roi));

        Rect roi_second = Rect(540,0,100,360);
        roi = Rect( 100,0,100,360 );
        tmp(roi_second).copyTo(stream(roi));
*/
        imshow("roi", stream);
        if(waitKey(30) >= 0) break;
    }
    // the camera will be deinitialized automatically in VideoCapture destructor
    return 0;
}
