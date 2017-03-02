//written in Feb, 7, 2017

//recieve capital L,R and sends serial to arduino when middle section's # of pixel become zero



#include "opencv2/opencv.hpp"
#include <time.h>
#include <fstream>
#include <iostream>
#include <string>



using namespace cv;
using namespace std;


int is_white(Mat frame, int row, int col);
void sleep( unsigned int mseconds );
char lanetrace(int average, int left_pixel, int right_pixel);
int computePixel(Mat frame, int startRow, int number);
int colCheck(Mat frame);
int middlePoint(Mat left, Mat right, int row);

int main(int, char**){

    ifstream instruction_file;
    ofstream flush;

    flush.open("instruction.txt");
    flush << "X";
    flush.close();

    VideoCapture cap(0); // open the default camera

    FILE *file;

    Mat stream;
    Mat frame( 100, 150, CV_8UC1 );

    int width = frame.cols;
    int height = frame.rows;

    int stateShift;
    int curveState = 0;
    int backban = 0;

    Mat stream_left ( height/5*3, width/2, CV_8UC1 );
    Mat stream_right ( height/5*3, width/2, CV_8UC1 );

    Mat full_left(height, width/2, CV_8UC1);
    Mat full_right(height, width/2, CV_8UC1);


    namedWindow("left",1);
    namedWindow("right",2);
    namedWindow("stream",3);

    char buff[5];
    char instruction, serial;

    while (1)  {
        
        instruction_file.open("instruction.txt", ios::in);
        if (instruction_file){
            instruction_file.get(instruction);
            instruction_file.close();
        }

        Mat stream;
        cap >> stream; // get a new frame from camera

        resize( stream, frame, Size( 150, 100 ), 0, 0);    
        cvtColor(frame, frame, COLOR_BGR2GRAY);
        threshold(frame, frame, 150, 255, THRESH_BINARY);

        Rect leftRect = Rect( 0, height/5*2, width/2, height/5*3 );
        stream_left = frame(leftRect);

        Rect rightRect = Rect(width/2, height/5*2, width/2, height/5*3);
        stream_right = frame(rightRect);

        Rect fullLeft = Rect( 0, 0, width/2, height );
        full_left = frame(fullLeft);

        Rect fullRight = Rect(width/2, 0, width/2, height);
        full_right = frame(fullRight);


        int left_pixel = 0, right_pixel = 0;
        int target_segment = 0;
        int leftRowCheck = 0, rightRowCheck = 0;

        left_pixel = computePixel(stream_left, 0, stream_left.rows);
        right_pixel = computePixel(stream_right, 0, stream_right.rows);

////target segment for sending L,R
        int average = (left_pixel + right_pixel)/2;

        if ( instruction == 'L' || instruction == 'R' ){
            int middle0 = middlePoint( full_left, full_right, 0 );
            int middle1 = middlePoint( full_left, full_right, full_right.rows/4 );
            int middle2 = middlePoint( full_left, full_right, full_right.rows/2 );

            cout << "First : " << midㄴle0 << "quarter : " << middle1 << ", middle : " << middle2 << endl;

            if ( instruction == 'L' ){
                target_segment = computePixel(stream_left, 35, 25);
                cout << "target_segment : " << target_segment << endl;
            }
            
            else if ( instruction == 'R' )        {
                target_segment = computePixel(stream_right, 35, 25);
                cout << "target_segment : " << target_segment << endl;
            }

            // start condition
            if ( target_segment == 0 && backban == 0 && curveState == 0 ) {
                serial = instruction;
                curveState = 1;
                cout << "target segment == 0, Turn start" << endl;
            }
            //during curve
            else if (curveState == 1){
                cout << "colCheck : " << colCheck(frame) << endl;

                if ( colCheck(frame) == 0 && target_segment != 0 && middle1*middle2!=0)  {
                    cout << " normalized, exit Turn state" << endl;
                    curveState = 2;
                    backban = 1;
                    //sleep(1000000);
                    serial = lanetrace(average, left_pixel, right_pixel);
                }
                else {
                    serial = instruction;
                    cout << "Turning..  " << endl;
                    backban = 1;
                }
            }
            //not curve
            else if (curveState == 0){
                if ( instruction == 'L' ) 
                    serial = 'l';
                else if ( instruction == 'R' ) 
                    serial = 'r';
                cout << "curveState == 0" <<endl;
            }
            else if ( curveState == 2 ){
                if ( instruction == 'L' ) 
                    serial = 'r';
                else if ( instruction == 'R' ) 
                    serial = 'l';
                cout << "curveState == 0" <<endl;
            }

        }

        else if (instruction == 'S' || instruction == 'H'){
            serial = 'S';
            backban = 0;
        }ㄴ
        else if (instruction == 'F'){
            curveState = 0;
            serial = lanetrace(average, left_pixel, right_pixel);
            backban = 0;
        }

        if ( instruction != 'X' ){
           file = fopen("/dev/ttyUSB0", "w");
           fprintf(file, "%c", serial);
           fclose(file);
        }

        cout << "receive : " << instruction << ", send : " << serial << endl;
        cout << "left : " << left_pixel << ", right : " << right_pixel <<endl;
        cout << " ------------------------------------" << endl;

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


void sleep ( unsigned int mseconds ) {

    clock_t goal = mseconds + clock();
    while ( goal > clock());

}

char lanetrace(int average, int left_pixel, int right_pixel){

    char direction;

    if ( average - 50 > left_pixel )
        direction = 'l';
    else if ( average - 50 > right_pixel )
        direction = 'r';
    else 
        direction = 'f';

    return direction;
    
}

int computePixel(Mat frame, int startRow, int number){
    int pixel = 0;

    for ( int row = startRow; row < startRow+number; row++) {
        for ( int col = 0; col < frame.cols; col++) {
            if ( is_white(frame, row, col)) {
                pixel++;
            }
            else 
                frame.at<uchar>(row, col) == 127;
        }
    }

    return pixel;
}


int middlePoint(Mat left, Mat right, int row){

    int point = 0;
    int num = 0;
    int leftPoint, rightPoint;

    for ( int col = 0; col < left.cols; col++ )
        if ( is_white(left, row, col)){
            point += col;
            num++;
        }

    if ( num != 0 ) 
        leftPoint = point / num;
    else 
        leftPoint = 0;
    

    point = 0;
    num = 0;

    for ( int col = 0; col < right.cols; col++ )
        if ( is_white(right, row, col)){
            point += col;
            num++;
        }

    if ( num != 0 ) 
        rightPoint = point / num + 75;
    else 
        rightPoint = 0;
    
    if (rightPoint * leftPoint != 0) 
        return (leftPoint+rightPoint)/2;
    else 
        return 0;
}



int colCheck(Mat frame){
    //int allExist = 1;
    int pixel = 0;

    for ( int row = 0; row < frame.rows; row++ ){
        if ( is_white(frame, row, 75)){
            pixel++;
            frame.at<uchar>(row, 75) = 123;
        }
    }
    return pixel;
}