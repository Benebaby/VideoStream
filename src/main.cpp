#include <iostream>
#include <cstdlib>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include "ThreadCam.h"
//---------------------------------------------------------------------------
int main()
{
    cv::Size camSize(1280, 720);
    cv::Size previewSize(1600, 900);
    cv::Mat Frame1, Frame2, FrameCon1;
    cv::Mat Frame3, Frame4, FrameCon2, FrameTotal, FrameTotalResized;
    cv::Mat FrameE(camSize.height, camSize.width, CV_8UC3, cv::Scalar(128,128,128));  //a gray frame
    //
    /// In this example, 4 cameras are used. This is not necessary. 
    /// You can control any number, one, two or, for example, six. It does not matter.    
    //
    ThreadCam *Grb1, *Grb2, *Grb3;

    Grb1 = new ThreadCam();
    Grb2 = new ThreadCam();
    Grb3 = new ThreadCam();

    Grb1->Init("videotestsrc ! video/x-raw, width=1280, height=720,framerate=30/1 ! clockoverlay ! videoscale ! videoconvert ! appsink");      //Camera 1
    Grb2->Init("videotestsrc ! video/x-raw, width=1280, height=720,framerate=30/1 ! clockoverlay ! videoscale ! videoconvert ! appsink");      //Camera 2
    Grb3->Init("videotestsrc ! video/x-raw, width=1280, height=720,framerate=30/1 ! clockoverlay ! videoscale ! videoconvert ! appsink");      //Camera 3
    // use a gray frame to indicate an empty field (lost connection with camera, for instance)
    //
    ///  be sure every frame has the same size!
    ///  hconcat and vconcat expect equal width and height
    ///  otherwise OpenCV throws exceptions
    //
    while(true)
    {
        //get the new frame
        Grb1->GetFrame(Frame1);
        if(!Frame1.empty()){
            ///place here your time consuming algorithms
        }
        else FrameE.copyTo(Frame1);

        //get the new frame
        Grb2->GetFrame(Frame2);
        if(!Frame2.empty()){
            ///place here your time consuming algorithms
        }
        else FrameE.copyTo(Frame2);

        //get the new frame
        Grb3->GetFrame(Frame3);
        if(!Frame3.empty()){
            ///place here your time consuming algorithms
        }
        else FrameE.copyTo(Frame3);

        //concat the four frames
        cv::hconcat(Frame1,Frame2,FrameCon1);
        cv::hconcat(Frame3,FrameE,FrameCon2);
        cv::vconcat(FrameCon1,FrameCon2,FrameTotal);
        cv::resize(FrameTotal, FrameTotalResized, previewSize);

        //show the result
        cv::imshow("Camera Preview",FrameTotalResized);
        char esc = cv::waitKey(5);
        if(esc == 27) break;
    }
    /// Gracefully, terminate the threads.
    Grb1->Quit();
    Grb2->Quit();
    Grb3->Quit();

    delete Grb1;
    delete Grb2;
    delete Grb3;

    return 0;
}