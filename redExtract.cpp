
#include <iostream>
#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;

const int red_threshold = 230;
const int other_threshold = 150;

int is_red(Mat frame, int row, int col);
void draw_black(Mat frame, int row, int col);

int main(int argc, const char * argv[]) {

    VideoCapture capture = VideoCapture(1);

    int red_position_x, red_position_y, red_count;
    red_position_x = red_position_y = red_count = 0;

    while(waitKey(1) != 'q') {
        Mat frame;
        capture >> frame;

        for(int row = 0; row < frame.rows; row++) {
            for(int col = 0; col < frame.cols; col++) {
                if(!is_red(frame, row, col)) {
                    draw_black(frame, row, col);
                } else {
                    red_position_x += col;
                    red_position_y += row;
                    red_count += 1;
                }
            }
        }

        if ( redcount != 0 ) {
            red_position_x /= red_count;
            red_position_y /= red_count;
        }
        
        imshow("frame", frame);
        std::cout << red_position_x << ", " << red_position_y << std::endl;

    }

    return 0;
}

int is_red(Mat frame, int row, int col) {
    return
            frame.at<Vec3b>(row, col)[0] < other_threshold &&
            frame.at<Vec3b>(row, col)[1] < other_threshold &&
            frame.at<Vec3b>(row, col)[2] > red_threshold;
}

void draw_black(Mat frame, int row, int col) {
    for(int i = 0; i < 3; i++) {
        frame.at<Vec3b>(row, col)[i] = 0;
    }
}