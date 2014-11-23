#ifndef HOGDETECTOR_H
#define HOGDETECTOR_H

#include <iostream>
#include <fstream>
#include <string>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>
using namespace cv;
using namespace std;


class HogDetector
{
public:
    HogDetector();
    ~HogDetector();

    void detectCar(Mat &frame, int index);
    void detectPerson(Mat &frame, int index);
    void visualize(Mat &frame);
    vector< float > findHogFeature(Mat &frame);
    HOGDescriptor hog;

};

#endif // HOGDETECTOR_H
