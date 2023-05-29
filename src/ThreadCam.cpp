#include "ThreadCam.h"
#include <mutex>
//---------------------------------------------------------------------------------
ThreadCam::ThreadCam(): GrabOpen(false), GrabStop(false), GrabTag(0), Tag(0), PrevTag(0)
{
    //ctor
}
//----------------------------------------------------------------------------------
ThreadCam::~ThreadCam()
{
    GrabOpen.store(false);
    cap.release();
}
//----------------------------------------------------------------------------------
void ThreadCam::Init(std::string dev, cv::VideoCaptureAPIs flags)
{
    device = dev;
    api = flags;
    ThGr = std::thread([=] { GrabThread(); });
}
//----------------------------------------------------------------------------------
void ThreadCam::Quit(void)
{
    GrabStop.store(true);
    ThGr.join();                //wait until the grab thread ends
}
//----------------------------------------------------------------------------------
void ThreadCam::GrabThread(void)
{
    int Try=0;
    cv::Mat tmp;

    while(!cap.isOpened()){
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        cap.open(device, api);
    }
    GrabOpen.store(true);

    while(GrabStop.load() == false){
        if(!cap.read(tmp)){
            std::this_thread::sleep_for(std::chrono::milliseconds(15));
            Try++;
            if(Try>250){    //after 3.5 Sec no sound
                //lost connection, retry to open again.
                cap.release();
                GrabOpen.store(false);
                while(!cap.isOpened()){
                    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                    cap.open(device, api);
                }
                GrabOpen.store(true);
                Try=0;
            }
        }
        else{
            std::lock_guard<std::mutex> lock(mtx);
            tmp.copyTo(mm);
            GrabTag.store(++Tag);
            Try=0;
        }
    }
}
//----------------------------------------------------------------------------------
void ThreadCam::GetFrame(cv::Mat& m)
{
    cv::Mat EmptyMat(mm.rows, mm.cols, CV_8UC3, cv::Scalar(50,50,50));
    std::chrono::steady_clock::time_point Tyet, Tgrab;

    //quick check to see if there is a new frame
    if(GrabTag.load()!=PrevTag){
        {
            std::lock_guard<std::mutex> lock(mtx);
            mm.copyTo(m);
        }
        PrevTag=GrabTag.load();
    }
    else if(GrabOpen.load()==false){
        //lost connection -> send empty frame
        EmptyMat.copyTo(m);
    }
}