#include <cstdint>
#include <vector>
#include <stdio.h>
#include <ctime>

#include <mongocxx/client.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>

#include <iostream>
#include "opencv2/opencv.hpp"

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>

mongocxx::uri uri("mongodb://192.168.0.16");
mongocxx::client client(uri);

using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::open_document;
using namespace std;
using namespace cv;

int focus_threshold = 100;
int other_threshold = 100;

int is_blue(Mat frame, int row, int col);
int is_red(Mat frame, int row, int col);
void fill_black(Mat frame, int row, int col);
void create_update(int location_x, int location_y, char color);

//db connect
mongocxx::database db = client["location"];
mongocxx::collection Blue_point = db["Blue_point"];
mongocxx::collection Red_point  = db["Red_point"];

int main(int, char **) {

//    Location.drop();

    //image prcss
    VideoCapture capture = VideoCapture(0);

    int red_position_x, red_position_y, red_count;
    red_position_x = red_position_y = red_count = 0;

    int blue_position_x, blue_position_y, blue_count;
    blue_position_x = blue_position_y = blue_count = 0;

    while(waitKey(1) != 'q') {
        Mat frame;
        capture >> frame;

std::clock_t    start;
start = std::clock();

        for(int row = 0; row < frame.rows; row++)
            for(int col = 0; col < frame.cols; col++) {
      
                if (is_red(frame, row, col)) {
                    red_position_x += col;
                    red_position_y += row;
                    red_count += 1;
                }
                else if (is_blue(frame, row, col)) {

                    blue_position_x += col;
                    blue_position_y += row;
                    blue_count += 1;
                }
                else fill_black(frame, row, col);
            }

        if (red_count != 0) {
            red_position_x /= red_count;
            red_position_y /= red_count;
        }

        if (blue_count != 0) {
            blue_position_x / blue_count;
            blue_position_y / blue_count;
        }

        imshow("frame", frame);

std::clock_t  middle;
middle = std::clock();

std::cout << "Time: " << (std::clock() - start) / (double)(CLOCKS_PER_SEC / 100$
        create_update(red_position_x, red_position_y, 'r');
        create_update(blue_position_x, blue_position_y, 'b');
std::cout << "Time: " << (std::clock() - middle) / (double)(CLOCKS_PER_SEC / 10$
    }
    return 0;

}


int is_red(Mat frame, int row, int col) {
    return
            frame.at<Vec3b>(row, col)[0] < other_threshold &&
            frame.at<Vec3b>(row, col)[1] < other_threshold &&
            frame.at<Vec3b>(row, col)[2] > focus_threshold;
}

int is_blue(Mat frame, int row, int col) {
    return
            frame.at<Vec3b>(row, col)[0] > focus_threshold &&
            frame.at<Vec3b>(row, col)[1] < other_threshold &&
            frame.at<Vec3b>(row, col)[2] < other_threshold;
}

void fill_black(Mat frame, int row, int col) {

    for(int i = 0; i < 3; i++) {
        frame.at<Vec3b>(row, col)[i] = 0;
    }
}

void create_update(int location_x, int location_y, char color) {

    bsoncxx::builder::stream::document document{};

    if (color == 'r'){
        auto collection = client["localDB"]["Red_point"];
        Red_point.insert_one(document.view());
        std::cout << "Red : " << location_x << ", " << location_y <<std::endl;
    }
    else if (color == 'b'){
        auto collection = client["localDB"]["Blue_point"];
        Blue_point.insert_one(document.view());
        std::cout << "Blue : " << location_x << ", " << location_y <<std::endl;
    }
    else {};

    document << "x" << location_x
             << "y" << location_y;

}
