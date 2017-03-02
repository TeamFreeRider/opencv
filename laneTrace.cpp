//written in Feb, 1, 2017

// receive capital L,R slightly before turn point
// when the number of pixels become lower than 150, sends L,R to arduino



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

    Mat stream_left ( height/5*3, width/2, CV_8UC1 );
    Mat stream_right ( height/5*3, width/2, CV_8UC1 );

    namedWindow("left",1);
    namedWindow("right",2);
    namedWindow("stream",3);

    char buff[5];
    char instruction, serial;

    while (1)  {
        
        instruction_file.open("instruction.txt", ios::in);
        if (instruction_file){
            instruction_file.get(instruction);
            cout << instruction << endl;
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
        

        if ( instruction == 'L' ){
            if ( left_pixel < 150 ) 
                serial = 'L';
            else 
                serial = lanetrace(average, left_pixel, right_pixel);
        }
        
        else if (instruction == 'R' ){
            if (right_pixel < 150 )
                serial = 'R';
            else 
                serial = lanetrace(average, left_pixel, right_pixel);
        }

        else if (instruction == 'S')
            serial = 'S';

        else 
            serial = lanetrace(average, left_pixel, right_pixel);
        
        file = fopen("/dev/ttyUSB0", "w");
        fprintf(file, "%c", serial);
        fclose(file);

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