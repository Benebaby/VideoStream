#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <stdio.h>
#include <chrono>
int main(int, char**)
{
    cv::Mat frame;
    cv::Mat resized_frame;
    cv::VideoCapture cap;
    std::string address = "http://raspberrypi:8888/video";
    cap.open(address);
    // check if we succeeded
    if (!cap.isOpened()) {
        std::cerr << "ERROR! Unable to open camera\n";
        return -1;
    }
    cv::namedWindow("IP Stream", cv::WINDOW_NORMAL);
    std::cout << "Start grabbing \n Press any key to terminate" << std::endl;
    auto time = std::chrono::steady_clock::now();
    unsigned int fps = 0;
    bool first_frame = true;
    while(true)
    {
        fps++;
        auto current_time = std::chrono::steady_clock::now();
        auto Duration = std::chrono::duration_cast<std::chrono::microseconds>(current_time-time);
        if(Duration.count() > 1000000)
        {
            std::cout << "FPS: " << fps << std::endl;
            time = std::chrono::steady_clock::now();
            fps = 0;
        }
        // wait for a new frame from camera and store it into 'frame'
        cap.read(frame);
        if(first_frame)
        {
            std::cout << "RESOLUTION: " << frame.cols << "x" << frame.rows << std::endl;
        }
        // check if we succeeded
        if (frame.empty()) {
            std::cerr << "ERROR! blank frame grabbed\n";
            break;
        }
        cv::resize(frame, resized_frame, cv::Size(4056, 1080), cv::INTER_CUBIC);
        if(first_frame)
        {
            cv::imwrite(ASSET_PATH"first_frame.jpg", resized_frame);
            first_frame = false;
        }
        // show live and wait for a key with timeout long enough to show images
        cv::imshow("IP Stream", resized_frame);
        if (cv::waitKey(5) >= 0)
            break;
    }
    // the camera will be deinitialized automatically in VideoCapture destructor
    return 0;
}