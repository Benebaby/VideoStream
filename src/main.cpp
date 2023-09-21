#include <iostream>
#include <cstdlib>
#include <chrono>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include "ThreadCam.h"
using namespace std::chrono_literals;
//---------------------------------------------------------------------------
int main()
{
    cv::Size camSize(1600, 1200);
    cv::Size previewSize(1920, 1440);
    cv::VideoCapture videoCapture;
    cv::VideoWriter videoWriter;
    cv::Mat Frame1, Frame2, FrameCon1;
    cv::Mat Frame3, Frame4, FrameCon2, FrameTotal, FrameTotalPreview;
    cv::Mat FrameE(camSize.height, camSize.width, CV_8UC3, cv::Scalar(50,50,50));  //a gray frame
    bool record = false;
    //
    /// In this example, 4 cameras are used. This is not necessary. 
    /// You can control any number, one, two or, for example, six. It does not matter.    
    //
    ThreadCam *Grb1, *Grb2, *Grb3, *Grb4;

    Grb1 = new ThreadCam();
    Grb2 = new ThreadCam();
    Grb3 = new ThreadCam();
    Grb4 = new ThreadCam();
    Grb1->Init("tcpclientsrc host=192.168.178.151 port=8888 ! jpegdec ! videoconvert ! appsink", cv::CAP_GSTREAMER);      //Camera 1
    Grb2->Init("tcpclientsrc host=192.168.178.136 port=8888 ! jpegdec ! videoconvert ! appsink", cv::CAP_GSTREAMER);      //Camera 2
    Grb3->Init("tcpclientsrc host=192.168.178.137 port=8888 ! jpegdec ! videoconvert ! appsink", cv::CAP_GSTREAMER);      //Camera 3
    Grb4->Init("tcpclientsrc host=192.168.178.150 port=8888 ! jpegdec ! videoconvert ! appsink", cv::CAP_GSTREAMER);      //Camera 4
    // use a gray frame to indicate an empty field (lost connection with camera, for instance)
    //
    ///  be sure every frame has the same size!
    ///  hconcat and vconcat expect equal width and height
    ///  otherwise OpenCV throws exceptions
    //
    auto time = std::chrono::steady_clock::now();
    auto captureTime = std::chrono::steady_clock::now();
    uint32_t fps = 0;
    uint32_t writtenfps = 0;
    cv::namedWindow("Camera Preview", cv::WINDOW_AUTOSIZE);

    while(true)
    {
        fps++;
        auto current_time = std::chrono::steady_clock::now();
        auto Duration = std::chrono::duration_cast<std::chrono::microseconds>(current_time-time);
        char keyPressed = cv::pollKey();
        if(keyPressed == 27) 
            break;
        if(Duration.count() > 1000000)
        {
            std::cout << "FPS: " << fps << std::endl;
            if(record)
                std::cout << "Written-FPS: " << writtenfps << std::endl;
            time = std::chrono::steady_clock::now();
            fps = 0;
            writtenfps = 0;
        }
        //get the new frame
        Grb1->GetFrame(Frame1);
        if(!Frame1.empty()){}
        else FrameE.copyTo(Frame1);

        //get the new frame
        Grb2->GetFrame(Frame2);
        if(!Frame2.empty()){}
        else FrameE.copyTo(Frame2);

        //get the new frame
        Grb3->GetFrame(Frame3);
        if(!Frame3.empty()){}
        else FrameE.copyTo(Frame3);

        //get the new frame
        Grb4->GetFrame(Frame4);
        if(!Frame4.empty()){}
        else FrameE.copyTo(Frame4);

        cv::hconcat(Frame1,Frame2,FrameCon1);
        cv::hconcat(Frame3,Frame4,FrameCon2);
        cv::vconcat(FrameCon1,FrameCon2,FrameTotal);
        cv::resize(FrameTotal, FrameTotalPreview, previewSize, cv::INTER_NEAREST);
        if(keyPressed == 114 || keyPressed == 82)
        {
            if(record)
            {
                record = false;
                videoCapture.release();
                videoWriter.release();
            }
            else
            {
                record = true;
                time_t tt = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
                tm local_tm = *localtime(&tt);
                std::string name = std::to_string(local_tm.tm_mday)+"-"+std::to_string(local_tm.tm_mon + 1)+"-"+std::to_string(local_tm.tm_year + 1900)+"_"+std::to_string(local_tm.tm_hour)+"-"+std::to_string(local_tm.tm_min)+"-"+std::to_string(local_tm.tm_sec)+".mp4";
                std::cout << "Start Recording: " << name << std::endl;
                videoCapture = cv::VideoCapture("appsrc ! decodebin ! x264enc ! mp4mux ! filesink location="+name, cv::CAP_GSTREAMER);
                videoCapture.set(3, FrameTotal.cols);
                videoCapture.set(4, FrameTotal.rows);
                int32_t fourcc = cv::VideoWriter::fourcc('M', 'P', '4', 'V');
                videoWriter = cv::VideoWriter(name, fourcc, 30.0, cv::Size(FrameTotal.cols,FrameTotal.rows));
            }
        }

        if(record)
        {
            cv::rectangle(FrameTotalPreview, cv::Point(0, 0), cv::Point(FrameTotalPreview.cols, 10), cv::Scalar(0, 255, 0), -1);
            cv::rectangle(FrameTotalPreview, cv::Point(0, (FrameTotalPreview.rows - 10)), cv::Point(FrameTotalPreview.cols, FrameTotalPreview.rows), cv::Scalar(0, 255, 0), -1);
            cv::rectangle(FrameTotalPreview, cv::Point(0, 0), cv::Point(10, FrameTotalPreview.rows), cv::Scalar(0, 255, 0), -1);
            cv::rectangle(FrameTotalPreview, cv::Point((FrameTotalPreview.cols - 10), 0), cv::Point(FrameTotalPreview.cols, FrameTotalPreview.rows), cv::Scalar(0, 255, 0), -1);
            auto currentCaptureTime = std::chrono::steady_clock::now();
            auto Duration = std::chrono::duration_cast<std::chrono::microseconds>(currentCaptureTime-captureTime);
            if(Duration.count() >= 33333){
                videoWriter.write(FrameTotal);
                writtenfps++;
                currentCaptureTime = std::chrono::steady_clock::now();
            }
        }
        
        if (cv::getWindowProperty("Camera Preview", cv::WND_PROP_VISIBLE) > 0)
        {
            cv::imshow("Camera Preview",FrameTotalPreview);
        }
        else
        {
            cv::destroyAllWindows();
            break;
        }
    }
    /// Gracefully, terminate the threads.
    Grb1->Quit();
    Grb2->Quit();
    Grb3->Quit();
    Grb4->Quit();

    delete Grb1;
    delete Grb2;
    delete Grb3;
    delete Grb4;

    return 0;
}