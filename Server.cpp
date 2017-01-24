/*
 *   C++ UDP socket server for live image upstreaming
 *   Modified from http://cs.ecs.baylor.edu/~donahoo/practical/CSockets/practical/UDPEchoServer.cpp
 *   Copyright (C) 2015
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "PracticalSocket.h" // For UDPSocket and SocketException
#include <iostream>          // For cout and cerr
#include <cstdlib>           // For atoi()
#include <fstream>

#define BUF_LEN 65540 // Larger than maximum UDP packet size

#include "config.h"

#include <cstdint>
#include <vector>
#include <stdio.h>
#include <ctime>

#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;

int focus_threshold = 100;
int other_threshold = 100;

int is_blue(Mat frame, int row, int col);
int is_red(Mat frame, int row, int col);
void fill_black(Mat frame, int row, int col);
void writeText( int red_x, int red_y, int blue_x, int blue_y );
void editPortion( int *x, int *y );


//create an instance of ofstream, and opens text file
const char *path = "/Users/Celine/Documents/Projects/FreeRider/TheServer/locations.txt";
ofstream location_file;

int main(int argc, char * argv[]) {

    if (argc != 2) { // Test for correct number of parameters
        cerr << "Usage: " << argv[0] << " <Server Port>" << endl;
        exit(1);
    }

    unsigned short servPort = atoi(argv[1]); // First arg:  local port

    namedWindow("recv", CV_WINDOW_AUTOSIZE);
    try {
        UDPSocket sock(servPort);

        char buffer[BUF_LEN]; // Buffer for echo string
        int recvMsgSize; // Size of received message
        string sourceAddress; // Address of datagram source
        unsigned short sourcePort; // Port of datagram source

        clock_t last_cycle = clock();

        while (1) {
            // Block until receive message from a client
            do {
                recvMsgSize = sock.recvFrom(buffer, BUF_LEN, sourceAddress, sourcePort);
            } while (recvMsgSize > sizeof(int));
            int total_pack = ((int * ) buffer)[0];

//            cout << "expecting length of packs:" << total_pack << endl;
            char * longbuf = new char[PACK_SIZE * total_pack];
            for (int i = 0; i < total_pack; i++) {
                recvMsgSize = sock.recvFrom(buffer, BUF_LEN, sourceAddress, sourcePort);
                if (recvMsgSize != PACK_SIZE) {
                    cerr << "Received unexpected size pack:" << recvMsgSize << endl;
                    continue;
                }
                memcpy( & longbuf[i * PACK_SIZE], buffer, PACK_SIZE);
            }

//            cout << "Received packet from " << sourceAddress << ":" << sourcePort << endl;
 
            Mat rawData = Mat(1, PACK_SIZE * total_pack, CV_8UC1, longbuf);
            Mat frame = imdecode(rawData, CV_LOAD_IMAGE_COLOR);
            if (frame.size().width == 0) {
                cerr << "decode failure!" << endl;
                continue;
            }

//variables initialize...
    int red_position_x, red_position_y, red_count;
    red_position_x = red_position_y = red_count = 0;

    int blue_position_x, blue_position_y, blue_count;
    blue_position_x = blue_position_y = blue_count = 0;

//image processing program added here..

/*
std::clock_t    start;
start = std::clock();
*/


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
                else {fill_black(frame, row, col);}
            }

        if (red_count != 0) {
            red_position_x /= red_count;
            red_position_y /= red_count;
        }

        if (blue_count != 0) {
            blue_position_x /= blue_count;
            blue_position_y /= blue_count;
        }



        editPortion(&red_position_x, &red_position_y);
        editPortion(&blue_position_x, &blue_position_y);

        red_position_y = 28 - red_position_y;
	blue_position_y = 28 - blue_position_y;

	red_position_x += 1;
	blue_position_x += 1;


std::cout << "red : " << red_position_x << ", " << red_position_y << "// blue : " << blue_position_x << ", " << blue_position_y << std::endl;

	writeText(red_position_x, red_position_y, blue_position_x, blue_position_y );

//std::cout << "Time: " << (std::clock() - start) / (double)(CLOCKS_PER_SEC / 1000) << "ms" << std::endl;

//        create_update(red_position_x, red_position_y, 'r');
//        create_update(blue_position_x, blue_position_y, 'b');
//std::cout << "Time: " << (std::clock() - middle) / (double)(CLOCKS_PER_SEC / 1000 << "ms" <<std::endl;


            imshow("recv", frame);

            free(longbuf);

//	    cout << " row : " << frame.rows << ", col : " << frame.cols <<endl;

            waitKey(1);
            clock_t next_cycle = clock();
            double duration = (next_cycle - last_cycle) / (double) CLOCKS_PER_SEC;

            last_cycle = next_cycle;
        }
    } catch (SocketException & e) {
        cerr << e.what() << endl;
        exit(1);

    }
    return 0;
}



int is_red(Mat frame, int row, int col) {
    return
            ((frame.at<Vec3b>(row, col)[0] < (frame.at<Vec3b>(row, col)[2] - 70)) &&
            (frame.at<Vec3b>(row, col)[1] < (frame.at<Vec3b>(row, col)[2] - 100)) &&
            frame.at<Vec3b>(row, col)[2] > 130) ||
	    ((frame.at<Vec3b>(row, col)[0] < 100) &&
            (frame.at<Vec3b>(row, col)[1] < 110) &&
            frame.at<Vec3b>(row, col)[2] > 150);
}

int is_blue(Mat frame, int row, int col) {
    return
            frame.at<Vec3b>(row, col)[0] > 100 &&
            (frame.at<Vec3b>(row, col)[1] < (frame.at<Vec3b>(row, col)[0] - 30)) &&
            (frame.at<Vec3b>(row, col)[2] < (frame.at<Vec3b>(row, col)[0] - 60));
}

void fill_black(Mat frame, int row, int col) {
    for(int i = 0; i < 3; i++) {
        frame.at<Vec3b>(row, col)[i] = 0;
    }
}


void writeText( int red_x, int red_y, int blue_x, int blue_y ){
    location_file.open(path);
//    location_file << "red : " << red_x << ", " << red_y << " / blue : " << blue_x << ", " << blue_y <<endl;

int data[4] = {red_x, red_y, blue_x, blue_y};


    for ( int i=0; i<4; i++ ){
	if (data[i] < 10)
	    location_file << "0" << data[i];
	else
	    location_file << data[i];
     }
	
    location_file.close();
}


void editPortion( int *x, int *y ){
    (*x) = (*x - 19) / 459.0 * 42; 
    (*y) = (*y - 19) / 292.0 * 28;
    return;
}
