#ifndef HOGDETECTOR_H
#define HOGDETECTOR_H

#include <iostream>
#include <fstream>
#include <string>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/ml/ml.hpp>

#include <QString>
#include <QDir>
#include <QDebug>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QTime>


using namespace cv;
using namespace std;


class HogDetector  : public QThread
{
Q_OBJECT
public:
    HogDetector(QObject *parent = 0);
    ~HogDetector();
    bool stop;
    QMutex mutex;
    QWaitCondition condition;

    QTime clock;
    void run();
    // if Stop = true, the thread will break
    // out of the loop, and will be disposed


    void detectCar(Mat &frame, int index);
    void detectPerson(Mat &frame, int index);
    void visualize(Mat &frame);

    vector< float > findHogFeature(Mat &frame);

    void train();
    void computeHOG(Mat &image, Mat &hogFeature);

    HOGDescriptor *hog;

    Size win_size = Size(64,32);
    Size block_size = Size(32,32);
    Size block_stride = Size(32,32);
    Size cell_size = Size(16,16);
    int no_bins = 21;

    CvSVMParams params;
    const int k_fold = 10;
    CvSVM* svm;

    // To communicate with Gui Thread
signals:
    void valueChanged(int);

public slots:


};

#endif // HOGDETECTOR_H
