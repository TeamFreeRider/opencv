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
int rowCheck(Mat frame);

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

    namedWindow("left",1);
    namedWindow("right",2);
    namedWindow("stream",3);

    char buff[5];
    char instruction, serial;

    while (1)  {
        
        cout << " ------------------------------------" << endl;
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

        int left_pixel = 0, right_pixel = 0;
        int target_segment = 0;
        int leftRowCheck = 0, rightRowCheck = 0;


        left_pixel = computePixel(stream_left, 0, stream_left.rows);
        right_pixel = computePixel(stream_right, 0, stream_right.rows);

////target segment for sending L,R
        int average = (left_pixel + right_pixel)/2;


        if ( instruction == 'L' || instruction == 'R' ){
//compute targeted segment pixel
            leftRowCheck = rowCheck(stream_left);
            rightRowCheck = rowCheck(stream_right);

            cout << "leftRowCheck : " << leftRowCheck << ", rightRowCheck : " << rightRowCheck << endl;

            if ( instruction == 'L' )
                target_segment = computePixel(stream_left, 35, 25);
            else if ( instruction == 'R' )        
                target_segment = computePixel(stream_right, 35, 25);
                
            // start condition
            if ( target_segment == 0 && backban == 0 && curveState == 0 ) {
                serial = instruction;
                curveState = 1;
                cout << "target segment == 0, Turn start" << endl;
            }
            //during curve
            else if (curveState == 1){
                if ( instruction == 'L' ){
                    if ( average > 650 && (leftRowCheck+rightRowCheck)/2 > 38 ){
                        cout << "Every Row Has White Pixel, normalized, exit Turn state" << endl;
                        curveState = 0;
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
                else if ( instruction == 'R' ){
                    if ( average > 650 && (leftRowCheck+rightRowCheck)/2 > 30 ){
                        cout << "Every Row Has White Pixel, normalized, exit Turn state" << endl;
                        curveState = 0;
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
            }
            //not curve
            else if (curveState == 0){
                serial = lanetrace(average, left_pixel, right_pixel);
                cout << "curveState == 0" <<endl;
            }
        }

        else if (instruction == 'S'){
            serial = 'S';
            backban = 0;
        }
        else if (instruction == 'F'){
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
        }
    }

    return pixel;
}

int rowCheck(Mat frame){
    //int allExist = 1;
    int pixel = 0;
    int numOfRow = 0;

    for ( int row = 0; row < frame.rows; row++ ){
        pixel = 0;
        for ( int col = 0; col < frame.cols; col++ )
            if ( is_white(frame, row, col)){
                pixel++;
                frame.at<uchar>(row, col) = 123;
                break;
            }
        if (pixel == 0) {
            //allExist = 0;
            numOfRow++;
        }
    }
    return 60 - numOfRow;
}