#ifndef PARKINGAPP_H
#define PARKINGAPP_H

#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QPoint>
#include <QRect>
#include <QLabel>
#include <QTime>
#include <QTimer>
#include <QProcess>
#include <QStringList>
#include <QStringListModel>
#include <QDir>

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "opencv2/contrib/contrib.hpp"
#include "opencv/ml.h"
#include "opencv2/objdetect/objdetect.hpp"

#include <player.h>
#include <detector/edgedetectors.h>
#include <detector/hogdetector.h>
#include <webdialog.h>
using namespace std;

namespace Ui {
  class ParkingApp;
}

class ParkingApp : public QMainWindow
{
    Q_OBJECT

public:
    explicit ParkingApp(QWidget *parent = 0);
    ~ParkingApp();
    //StructuredEdgeDetection structuredEdgeDetector;

private slots:
    //Display video frame in player UI
    void updatePlayerUI(QImage img, int index);
    void updateTime();

    void detectCar(Mat frame, int index);
    void testData2(int t, int k);

    void on_training_value_changed(int);

    //////////////////////////////////

    void onProcessReadyRead();

    void on_loadBtn_clicked();

    void on_openBtn_clicked();

    void on_calibrateBtn_toggled(bool checked);

    void on_saveCalibrateBtn_clicked();

    void on_monitorBtn_clicked();

    void on_saveVideoBtn_clicked();

    void on_videoFrameBtn_clicked();
    
    void on_videoControlBtn_clicked();
    
    void on_backwardBtn_clicked();

    void on_forwardBtn_clicked();

    void on_recordBtn_clicked();

    void on_pushButton_clicked();

    void on_Recording_clicked();

    void on_record_type_btn_toggled(bool checked);

    void on_openCalibrationBtn_clicked();

    //drag
    void onfireDragSignal();

    void on_training_btn_clicked();

    void on_videoList_doubleClicked(const QModelIndex &index);

    void on_personDetectBtn_clicked();

    void on_vehicleDetectBtn_clicked();

public slots:



private:
    Ui::ParkingApp *ui;
    Player* myPlayer;

    QTime clock;
    QTimer timer;
    QDateTime finishTime;

    QString filename;
    VideoWriter outputVideo;

    //
    QStringListModel *videoListModel;

    /////////////////
    QPixmap image;
    QImage  *imageObject;
    QGraphicsScene *scene;


    bool recording = false;
    int recording_index = 0;
    int recording_type = 0;
    char *recording_folder = "test";

    // execute python code
    QProcess *process;

    // detectors
    edgeDetectors *edgeDetector;
    HogDetector *hogDetector;

    // additional UI
    WebDialog * webdialog;

    void startVideo(QString temp);
    void readConfig();

signals:
    // drag
    void fireDragSignal();


    // QWidget interface
protected:
    void dragEnterEvent(QDragEnterEvent *e);
    void dropEvent(QDropEvent *e);
    void dragLeaveEvent(QDragEnterEvent *e);

};

#endif // MAINWINDOW_H
