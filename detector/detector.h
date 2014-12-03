#ifndef DETECTOR_H
#define DETECTOR_H

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/video/video.hpp"
#include <opencv2/objdetect/objdetect.hpp>

#include <iostream>
#include <fstream>
#include <string>

using namespace std;
using namespace cv;

class Detector
{

    cv::BackgroundSubtractorMOG2 bg;   //int history, float varThreshold, bool bShadowDetection=true


    public:

    Detector();
    //Destructor
    ~Detector();

       void init();
       void detect(Mat &frame, int num);
       void detect2(Mat &frame, int num);
       void update();
       void DIFF2(char *path);
       void GMM(char *path);
};

#endif // DETECTOR_H
