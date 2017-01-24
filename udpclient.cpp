
#include "PracticalSocket.h"      // For UDPSocket and SocketException
#include <iostream>               // For cout and cerr
#include <cstdlib>                // For atoi()

using namespace std;

#include "opencv2/opencv.hpp"
using namespace cv;
#include "config.h"


int main(int argc, char * argv[]) {
    if ((argc < 3) || (argc > 3)) { // Test for correct number of arguments
        cerr << "Usage: " << argv[0] << " <Server> <Server Port>\n";
        exit(1);
    }

    string servAddress = argv[1]; // First arg: server address
    unsigned short servPort = Socket::resolveService(argv[2], "udp");

    try {
        UDPSocket sock;
        int jpegqual =  ENCODE_QUALITY; // Compression Parameter


        Mat frame,cropped, send;
        vector < uchar > encoded;
        VideoCapture cap(0); // Grab the camera
        namedWindow("send", CV_WINDOW_AUTOSIZE);
        if (!cap.isOpened()) {
            cerr << "OpenCV Failed to open camera";
            exit(1);
        }

        clock_t last_cycle = clock();
        while (1) {
            cap >> frame;
            if(frame.size().width==0)continue;//simple integrity check; skip er$
            resize(frame, cropped, Size(640, 480), 0, 0, INTER_LINEAR);

            Rect mapRoi = Rect(69, 13, 495, 330);
            Mat send = cropped(mapRoi);


            vector < int > compression_params;
            compression_params.push_back(CV_IMWRITE_JPEG_QUALITY);
            compression_params.push_back(jpegqual);

            imencode(".jpg", send, encoded, compression_params);
            imshow("send", send);
            int total_pack = 1 + (encoded.size() - 1) / PACK_SIZE;

            int ibuf[1];
            ibuf[0] = total_pack;
            sock.sendTo(ibuf, sizeof(int), servAddress, servPort);

            for (int i = 0; i < total_pack; i++)
                sock.sendTo( & encoded[i * PACK_SIZE], PACK_SIZE, servAddress, $

            waitKey(FRAME_INTERVAL);

            clock_t next_cycle = clock();
            double duration = (next_cycle - last_cycle) / (double) CLOCKS_PER_S$
            cout << "\teffective FPS:" << (1 / duration) << " \tkbps:" << (PACK$

            cout << next_cycle - last_cycle;
            last_cycle = next_cycle;
        }
        // Destructor closes the socket

    } catch (SocketException & e) {
        cerr << e.what() << endl;
        exit(1);
    }

    return 0;
}


