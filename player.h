#ifndef PLAYER_H
#define PLAYER_H


#include <QMutex>
#include <QThread>
#include <QImage>
#include <QWaitCondition>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/video/video.hpp"
#include <opencv2/objdetect/objdetect.hpp>

#include <iostream>
#include <fstream>
#include <string>

#include <QFile>
#include <QCoreApplication>
#include <QTextStream>
#include <QStringList>

#include <stdio.h>


using namespace std;
using namespace cv;

class Player : public QThread
{ Q_OBJECT
  private:
     bool stop;
     QMutex mutex;
     QWaitCondition condition;
     Mat frame;
     int frameRate;
     VideoCapture *capture;
     Mat RGBframe;
     QImage img;


  signals:
     //Signal to output frame to be displayed
     void processedImage(const QImage &image,int index);
     void processImageMat(const Mat &frame,int index);
     void testData(const int, const int);


  protected:
     void run();
     void msleep(int ms);
  public:


     int type = 0;
     int currentImageIndex = 0;

     /*
     bool recording = false;
     int recording_index = 0;
     //char recording_folder[30] = {'t','e','s','t'};
     char *recording_folder = "test";
     bool Recording(string filename);

     */

     //Constructor
     Player(QObject *parent = 0);
     //Destructor
     ~Player();
     //Load a video from memory
     bool loadVideo(string filename);
     bool loadImages(string filename);
     //Play the video
     void Play();

     //Stop the video
     void Stop();
     //check if the player has been stopped
     bool isStopped() const;

     //set video properties
    void setCurrentFrame( int frameNumber);

    //Get video properties
    double getFrameRate();
    double getCurrentFrame();
    double getNumberOfFrames();

};
#endif // PLAYER_H
