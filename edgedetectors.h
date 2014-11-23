#ifndef EDGEDETECTORS_H
#define EDGEDETECTORS_H


#include <iostream>
#include <fstream>
#include <string>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>

using namespace cv;
class edgeDetectors
{
public:
    edgeDetectors();
    ~edgeDetectors();

    void detectCar(Mat frame, int index);
};

#endif // EDGEDETECTORS_H
