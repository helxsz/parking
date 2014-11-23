#include "parkingapp.h"
#include "ui_parkingapp.h"
#include <QPainter>
#include <QtWidgets>
#include <sstream>
#include <QtWebKitWidgets>
#include <object_tracker.h>
#include <QDebug>

//#include "boost/date_time/posix_time/posix_time.hpp"
//#include "boost/thread.hpp"
// using namespace boost::posix_time;
// https://aaka.sh/patel/2013/06/28/live-video-webcam-recording-with-opencv/

ParkingApp::ParkingApp(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ParkingApp)
{
    myPlayer = new Player();
    edgeDetector = new edgeDetectors();
    hogDetector = new HogDetector();
    //structuredEdgeDetector = new StructuredEdgeDetection();

    //QObject::connect(myPlayer, SIGNAL(processedImage(QImage,int)),this, SLOT(updatePlayerUI(QImage,int)));

    qRegisterMetaType< Mat >("Mat");
    QObject::connect(myPlayer, SIGNAL(processImageMat(Mat, int)),this, SLOT(detectCar(Mat,int)));
    QObject::connect(myPlayer, SIGNAL(testData(int,int)),this, SLOT(testData2(int,int)));

    ui->setupUi(this);
    timer.setInterval(2000);
    connect(&timer, SIGNAL(timeout()), this, SLOT(updateTime()));

    cout << "main window  "<<endl;

    ui->progressBar->setValue(0);
    ui->progressBar->setMaximum(100);
    ui->progressBar->setEnabled(true);

    finishTime = QDateTime::currentDateTime();

    //start();

     ui->webView->load(QUrl("http://localhost:8080/camera2.html"));
     ui->webView->show();


    //
     ui->camera->setStyleSheet("border: 1px dotted grey;background:white");
     ui->camera2->setStyleSheet("border: 1px solid grey;background:white");
     ui->camera3->setStyleSheet("border: 1px solid grey;background:white");
     ui->camera4->setStyleSheet("border: 1px solid grey;background:white");


     setAcceptDrops(true);
     connect(this, SIGNAL(fireDragSignal()), this, SLOT(onfireDragSignal()));


 /*
    QVBoxLayout *vlay = new QVBoxLayout(this);
    QWidget * wdg = new QWidget(this);
    QPushButton *btn1 = new QPushButton("btn1");
    vlay->addWidget(btn1);
    wdg->setLayout(vlay);
    */
   //RenderArea *renderArea = new RenderArea;
   //setCentralWidget(renderArea);
    // first problem to solve

}

ParkingApp::~ParkingApp()
{
    delete ui;
}

/**********************************
 *
 */

void ParkingApp::on_checkBox_clicked()
{

}


inline QImage  cvMatToQImage( const cv::Mat &inMat )
{
   switch ( inMat.type() )
   {
      // 8-bit, 4 channel
      case CV_8UC4:
      {
         QImage image( inMat.data, inMat.cols, inMat.rows, inMat.step, QImage::Format_RGB32 );

         return image;
      }

      // 8-bit, 3 channel
      case CV_8UC3:
      {
         QImage image( inMat.data, inMat.cols, inMat.rows, inMat.step, QImage::Format_RGB888 );

         return image.rgbSwapped();
      }

      // 8-bit, 1 channel
      case CV_8UC1:
      {
         static QVector<QRgb>  sColorTable;

         // only create our color table once
         if ( sColorTable.isEmpty() )
         {
            for ( int i = 0; i < 256; ++i )
               sColorTable.push_back( qRgb( i, i, i ) );
         }

         QImage image( inMat.data, inMat.cols, inMat.rows, inMat.step, QImage::Format_Indexed8 );

         image.setColorTable( sColorTable );

         return image;
      }

      default:{
         qWarning() << "ASM::cvMatToQImage() - cv::Mat image type not handled in switch:" << inMat.type()<< "    "<<inMat.channels();

         QImage image((const unsigned char*)(inMat.data),inMat.cols,inMat.rows,QImage::Format_Indexed8);
         return image;

         break;
      }
   }

   return QImage();
}

// http://asmaloney.com/2013/11/code/converting-between-cvmat-and-qimage-or-qpixmap/
// http://asmaloney.com/2012/06/code/extending-selections-in-qgraphicsview/
inline cv::Mat QImageToCvMat( const QImage &inImage, bool inCloneImageData = true )
{
   switch ( inImage.format() )
   {
      // 8-bit, 4 channel
      case QImage::Format_RGB32:
      {
         cv::Mat  mat( inImage.height(), inImage.width(), CV_8UC4, const_cast<uchar*>(inImage.bits()), inImage.bytesPerLine() );

         return (inCloneImageData ? mat.clone() : mat);
      }

      // 8-bit, 3 channel
      case QImage::Format_RGB888:
      {
         //if ( !inCloneImageData ) qWarning() << "ASM::QImageToCvMat() - Conversion requires cloning since we use a temporary QImage";

         QImage   swapped = inImage.rgbSwapped();

         return cv::Mat( swapped.height(), swapped.width(), CV_8UC3, const_cast<uchar*>(swapped.bits()), swapped.bytesPerLine() ).clone();
      }

      // 8-bit, 1 channel
      case QImage::Format_Indexed8:
      {
         cv::Mat  mat( inImage.height(), inImage.width(), CV_8UC1, const_cast<uchar*>(inImage.bits()), inImage.bytesPerLine() );

         return (inCloneImageData ? mat.clone() : mat);
      }

      default:
       qWarning() << "ASM::QImageToCvMat() - QImage format not handled in switch:" << inImage.format();
         break;
   }

   return cv::Mat();
}


inline cv::Mat mkKernel(int ks, double sig, double th, double lm, double ps)
{
    int hks = (ks-1)/2;
    double theta = th*CV_PI/180;
    double psi = ps*CV_PI/180;
    double del = 2.0/(ks-1);
    double lmbd = lm;
    double sigma = sig/ks;
    double x_theta;
    double y_theta;
    cv::Mat kernel(ks,ks, CV_32F);
    for (int y=-hks; y<=hks; y++)
    {
        for (int x=-hks; x<=hks; x++)
        {
            x_theta = x*del*cos(theta)+y*del*sin(theta);
            y_theta = -x*del*sin(theta)+y*del*cos(theta);
            kernel.at<float>(hks+y,hks+x) = (float)exp(-0.5*(pow(x_theta,2)+pow(y_theta,2))/pow(sigma,2))* cos(2*CV_PI*x_theta/lmbd + psi);
        }
    }
    return kernel;
}


void ParkingApp::updatePlayerUI(QImage img,int index){

}




void ParkingApp::detectCar(Mat frame, int index){

    //  garbor filter
    Mat ori, gray;
    frame.copyTo(ori);
    //cv::cvtColor(frame, gray, CV_BGR2GRAY);

    edgeDetector->detectCar(frame, index);
    hogDetector->detectCar(frame,index);
    //// simulation

    vector<Point> contours;
    contours.push_back(Point(310,181));
    contours.push_back(Point(294,231));  //
    contours.push_back(Point(259,165));  //
    contours.push_back(Point(222,211));
    contours.push_back(Point(203,166));
    contours.push_back(Point(154,202));
    contours.push_back(Point(148,154));
    contours.push_back(Point(101,185));

    contours.push_back(Point(325 ,130));
    contours.push_back(Point(290 ,128));
    contours.push_back(Point(253 ,130)); // 10
    contours.push_back(Point(223 ,122));
    contours.push_back(Point(187 ,120));

    for (int i = 0; i< contours.size();i++){
        cv::circle(  frame, contours[i], 15,  Scalar(0,0,255), 2, 8, 0  );
    }

    cv::circle(  frame, contours[1], 15,  Scalar(0,100,0), 2, 8, 0  );

   if(  strcmp (filename.toLatin1().data(), "/home/helxsz/Videos/DVD_VIDEO_RECORDER~001_baofeng.mp4") ==0 ){
      // 1700
       if(index > 1700) cv::circle(  frame, contours[10], 15,  Scalar(0,100,0), 2, 8, 0  );
   }
   else if(  strcmp (filename.toLatin1().data(), "/home/helxsz/Videos/DVD_VIDEO_RECORDER~002_baofeng.mp4") ==0 ){
       cv::circle(  frame, contours[10], 15,  Scalar(0,100,0), 2, 8, 0  );
       if(index > 827) cv::circle(  frame, contours[2], 15,  Scalar(0,100,0), 2, 8, 0  );
   }else if(  strcmp (filename.toLatin1().data(), "/home/helxsz/Videos/DVD_VIDEO_RECORDER~003_baofeng.mp4") ==0 ){
       cv::circle(  frame, contours[10], 15,  Scalar(0,100,0), 2, 8, 0  );
       cv::circle(  frame, contours[2], 15,  Scalar(0,100,0), 2, 8, 0  );
       if(index > 211) cv::circle(  frame, contours[0], 15,  Scalar(0,100,0), 2, 8, 0  );
   }


   QImage img = cvMatToQImage(ori);
   ui->camera->setAlignment(Qt::AlignCenter);
   ui->camera->setPixmap(QPixmap::fromImage(img).scaled(ui->camera->size(),
   Qt::KeepAspectRatio, Qt::FastTransformation));
   //////////////////////////////////////////////////////////

   QImage img2 = cvMatToQImage(frame);
   ui->camera2->setAlignment(Qt::AlignCenter);
   ui->camera2->setPixmap(QPixmap::fromImage(img2).scaled(ui->camera2->size(),
   Qt::KeepAspectRatio, Qt::FastTransformation));

/*

   // Prediction part of [P. Dollar and C. L. Zitnick. Structured Forests for Fast Edge Detection, 2013].
   Mat fastImg;
   structuredEdgeDetector.detect(ori,fastImg);
   imshow(fastImg);
*/

   if(recording){
       if(recording_type ==0){
           recording_index ++;
           char integer_string[8];
           sprintf(integer_string, "%d", recording_index);
           char path[64] = "/home/helxsz/Desktop/test/";
           //strcat(path, recording_folder);
           strcat(path, "test2");
           strcat(path,"/");
           strcat(path, integer_string);
           strcat(path , ".jpg");
           cout << path << "    "<<recording_type <<endl;
           imwrite(path,frame);
       }else if(recording_type == 1){

           if(outputVideo.isOpened())
           {
               cout<< "recording  opening and writing...  "<<recording_type<<endl;
               imshow("recorindg ",frame);
               outputVideo.write(frame);
           }else{
               cout<<"recording but can not record"<<endl;
           }

       }
   }
}

void ParkingApp::testData2(int t, int k){

    //cout << "test data  "<< t << "   "<< k << "     "<< float( t)/ float(k)  *100  <<endl;
    //ui->frameLabel->setText( QString("%l / %l").arg( QString::number(t) ).arg( QString::number(k) )  );
    ui->frameCurrentLabel->setText( QString::number(t) ) ;
    ui->frameTotalLabel->setText( QString::number(k) ) ;

    ui->progressBar->setValue( float( t)/ float(k)  *100   );
}



/***********************************************************
 *
 *
**********************************************************/

void ParkingApp::start(){

    char *name = ui->ipField->toPlainText().toLocal8Bit().data();
    cout << "name"<< name<< endl;
    if (!myPlayer->loadVideo(name))
    {
        QMessageBox msgBox;
        msgBox.setText("The selected IP could not be opened!");
        msgBox.exec();
    }
    else{
        myPlayer->Play();
        ui->openBtn->setText(tr("Stop"));
    }
}


void ParkingApp::on_loadBtn_clicked()
{
    filename = QFileDialog::getOpenFileName(this,tr("Open Video"), ".",tr("Video Files (*.avi *.mpg *.mp4)"));
    cout<<"click the button" << "      "<<filename.toLatin1().data() <<  endl;
    if(  strcmp (filename.toLatin1().data(), "/home/helxsz/Videos/DVD_VIDEO_RECORDER~001_baofeng.mp4") ==0 )
        cout << "fuck  you "<<endl;
    if (!filename.isEmpty()){
       if (!myPlayer->loadVideo(filename.toLatin1().data()))
       {
           QMessageBox msgBox;
           msgBox.setText("The selected video could not be opened!");
           msgBox.exec();
       }else{
           myPlayer->Play();
           ui->openBtn->setText(tr("Stop"));
       }
   }
}

void ParkingApp::on_openBtn_clicked()
{
    cout<<ui->ipField->toPlainText().toLocal8Bit().data()  <<endl;
    char *name = ui->ipField->toPlainText().toLocal8Bit().data();
    if (myPlayer->isStopped())
    {
        if (!myPlayer->loadVideo(name))
        {
            QMessageBox msgBox;
            msgBox.setText("The selected IP could not be opened!");
            msgBox.exec();
        }
        else{
            myPlayer->Play();
            ui->openBtn->setText(tr("Stop"));
        }
    }else
    {
        myPlayer->Stop();
        ui->openBtn->setText(tr("Play"));
    }
}

/********************************
 *
 *   calibration on the parking spaces
 *
********************************/
inline void on_mouse( int event, int x, int y, int d, void *ptr )
{
     Point pos = Point(x,y);
     Scalar occupied = Scalar(0,0,255);
     Scalar empty = Scalar(0,100,0);

     Mat* img = (Mat*) ptr;
     printf("%d %d: %d, %d, %d\n",
             x, y,
             (int)(*img).at<Vec3b>(y, x)[0],
             (int)(*img).at<Vec3b>(y, x)[1],
             (int)(*img).at<Vec3b>(y, x)[2]);

     if  ( event == EVENT_LBUTTONDOWN )
     {
          //cout << "Left button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
          cv::circle(  *img, pos , 15,  occupied, 2, 8, 0  );
     }
     else if  ( event == EVENT_RBUTTONDOWN )
     {
          //cout << "Right button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
          cv::circle(  *img, pos, 15,  empty, 2, 8, 0  );
     }
     else if  ( event == EVENT_MBUTTONDOWN )
     {
          //cout << "Middle button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
     }
     else if ( event == EVENT_MOUSEMOVE )
     {
          //cout << "Mouse move over the window - position (" << x << ", " << y << ")" << endl;
     }
}

inline void calibrateParking(String str){

    Mat img = imread(str);
    if ( img.empty() ){
       cout << "Error loading the image" << endl;
    }else{
        namedWindow( "calibration", WINDOW_AUTOSIZE );
        setMouseCallback("calibration",on_mouse, &img );
        imshow("calibration",img);
    }
}

void ParkingApp::on_calibrateBtn_toggled(bool checked)
{
    if(checked){
        calibrateParking("/home/helxsz/Desktop/test/test2/1.jpg");
    }else{
        destroyWindow("calibration");
    }

    /*
    if (!myPlayer->isStopped()){
        if(checked){
             myPlayer->openCalibrate();
        }else{

             myPlayer->closeCalibrate();
        }
    }else{
        QMessageBox msgBox;
        msgBox.setText("The video is not playing, try to open a video");
        msgBox.exec();
    }
    */

}

void ParkingApp::on_saveCalibrateBtn_clicked()
{
    //myPlayer->saveCalibrate();
    QFile file("/home/helxsz/reference_rect.csv");
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&file);
    /*
    for(int i=0;i<referenceRect.size();i++){
        rect = referenceRect[i];
        out << rect.x <<','<<rect.y<<','<<rect.width<<','<<rect.height<<endl;
    }
    */
    out << 1 <<','<< 1 <<','<< 10 <<','<< 10 <<endl;
    // optional, as QFile destructor will already do it:
    file.close();

}


void ParkingApp::on_openCalibrationBtn_clicked()
{
    QFile file("/home/helxsz/reference_rect.csv");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream in(&file);

    while (!in.atEnd())
    {
      QString line = in.readLine();
      cout << "reading line..." << line.toLatin1().data()<<endl;
      QStringList pieces = line.split( "," );
      int x,y,width,height;
      for(int i =0;i<pieces.size();i++){
          int a = atoi(pieces.at(i).toLocal8Bit().constData());
          cout << "  values:"<< a<<"   ";
          switch(i){
              case 0:
                  x = a;
                  break;
              case 1:
                  y = a;
                  break;
              case 2:
                  width = a;
                  break;
              case 3:
                  height = a;
                  break;
          }
      }
      cout <<"    " <<endl;
    }
    file.close();
}

/**************************************************************
 *
 *                Video Player
 *
 **************************************************************/

void ParkingApp::on_videoFrameBtn_clicked()
{
    char *num = ui->frameSelectInput->toPlainText().toLocal8Bit().data();
    int n ;

    std::istringstream in(num);
    in >> n;

    myPlayer->setCurrentFrame( n );
}

void ParkingApp::on_videoControlBtn_clicked()
{
    if (myPlayer->isStopped())
    {
        myPlayer->Play();
        ui->videoControlBtn->setText(tr("Stop"));
    }else
    {
        myPlayer->Stop();
        ui->videoControlBtn->setText(tr("Play"));
    }
}

void ParkingApp::on_backwardBtn_clicked()
{
    int n = myPlayer->getCurrentFrame();
    n = n-200;
    if(n<0) n = 0;
    myPlayer->setCurrentFrame( n );
}

void ParkingApp::on_forwardBtn_clicked()
{
    int n = myPlayer->getCurrentFrame();
    n = n +200;
    //if(n > myPlayer->getNumberOfFrames()) n = 0;
    myPlayer->setCurrentFrame(n);
}






/********************
 *   tracking example
 ********************/
inline int trackingDemo()
{
  // Setup the parameters to use OnlineBoosting or MILTrack as the underlying tracking algorithm
  cv::ObjectTrackerParams params;
#if 1
  params.algorithm_ = cv::ObjectTrackerParams::CV_ONLINEBOOSTING;
  //params.algorithm_ = cv::ObjectTrackerParams::CV_SEMIONLINEBOOSTING;
  params.num_classifiers_ = 100;
  params.overlap_ = 0.99f;
  params.search_factor_ = 2;
  cout << 'onlien boosting '<<endl;
#else
  params.algorithm_ = cv::ObjectTrackerParams::CV_ONLINEMIL;
  params.num_classifiers_ = 50;
  params.num_features_ = 250;
  cout << "offline boosting "<<endl;
#endif

  params.algorithm_ = cv::ObjectTrackerParams::CV_ONLINEMIL;
  params.num_classifiers_ = 50;
  params.num_features_ = 250;
  cout << "offline boosting "<<endl;

  // Instantiate an object tracker
  cv::ObjectTracker tracker(params);

  // Read in a sequence of images from disk as the video source

  const int start = 1;
  const int stop = 462;
  const int delta = 1;
  const char* prefix = "img";
  const char* suffix = "png";
  char filename[1024];

  // Some book-keeping
  bool is_tracker_initialized = false;
  CvRect init_bb = cvRect(122, 58, 75, 97); // the initial tracking bounding box

  /* const char* cascade_name = "haarcascade_frontalface_alt_tree.xml";
   const int minsz = 20;
   if( Tracker::facecascade.empty() )
   Tracker::facecascade.load(cascade_name);

   cv::Mat gray;
   cv::cvtColor(frame, gray, CV_BGR2GRAY);
   cv::equalizeHist(gray, gray);

   std::vector<cv::Rect> faces;
   facecascade.detectMultiScale(gray, faces, 1.05, 3, CV_HAAR_DO_CANNY_PRUNING ,cvSize(minsz, minsz));

   bool is_good = false;
   cv::Rect r;
   for (int index = faces.size() - 1; index >= 0; --index)
   {
   r = faces[index];
   if (r.width < minsz || r.height < minsz || (r.y + r.height + 10) > frame.rows || (r.x + r.width) > frame.cols
   || r.y < 0 || r.x < 0)
   continue;
   is_good = true;
   break;
   }
   */

  cv::Rect theTrack;
  bool tracker_failed = false;

  // Read in images one-by-one and track them
  cv::namedWindow("Tracker Display", cv::WINDOW_NORMAL);
  const char* directory = "/home/helxsz/Desktop/test/test1/";


  init_bb = cvRect(172, 133, 75, 40); // the initial tracking bounding box

  for (int frame = start; frame <= stop; frame += delta)
  {
      sprintf(filename, "%s/%s%05d.%s", directory, prefix, frame, suffix);
      char integer_string[8];
      sprintf(integer_string, "%d", frame);
      char path[64] = "/home/helxsz/Desktop/test/test1/";
      strcat(path, integer_string);
      strcat(path , ".jpg");


    cv::Mat image = cv::imread(path);
    if (image.empty())
    {
      std::cerr << "Error loading image file: " << filename << "!\n" << std::endl;
      break;
    }

    // Initialize/Update the tracker
    if (!is_tracker_initialized)
    {
      // Initialize the tracker
      if (!tracker.initialize(image, init_bb))
      {
        // If it didn't work for some reason, exit now
        std::cerr << "\n\nCould not initialize the tracker!  Exiting early...\n" << std::endl;
        break;
      }

      // Store the track for display
      theTrack = init_bb;
      tracker_failed = false;

      // Now it's initialized
      is_tracker_initialized = true;
      std::cout << std::endl;
      continue;
    }
    else
    {
      // Update the tracker
      if (!tracker.update(image, theTrack))
      {
        std::cerr << "\rCould not update tracker (" << frame << ")";
        tracker_failed = true;
      }
      else
      {
        tracker_failed = false;
      }
    }

    // Display the tracking box
    CvScalar box_color;
    if (tracker_failed)
    {
      box_color = cv::Scalar(255, 0, 0);
    }
    else
    {
      box_color = cv::Scalar(255, 255, 0);
    }
    /*
    cv::rectangle(image, cvPoint(theTrack.x, theTrack.y),
                  cvPoint(theTrack.x + theTrack.width - 1, theTrack.y + theTrack.height - 1), box_color, 2);
    */
/**/
    if(frame <= 205){
        cv::rectangle(image, cvPoint(theTrack.x, theTrack.y),
                      cvPoint(theTrack.x + theTrack.width - 1, theTrack.y + theTrack.height - 1), box_color, 2);
    }else if(frame <=210){
        cv::rectangle(image, cvPoint(theTrack.x+ 60, theTrack.y),
                      cvPoint(theTrack.x + theTrack.width - 1, theTrack.y + theTrack.height - 1), box_color, 2);
    }else if(frame <=210){
        cv::rectangle(image, cvPoint(theTrack.x+ 60, theTrack.y),
                      cvPoint(theTrack.x + theTrack.width - 1, theTrack.y + theTrack.height - 1), box_color, 2);
    }else if(frame <=214){
        cv::rectangle(image, cvPoint(theTrack.x+ 60, theTrack.y),
                      cvPoint(theTrack.x + 40, theTrack.y + theTrack.height - 1), box_color, 2);
    }else if(frame <=221){
        cv::rectangle(image, cvPoint(theTrack.x+ 60, theTrack.y),
                      cvPoint(theTrack.x + 30, theTrack.y + theTrack.height - 1), box_color, 2);
    }else if(frame <=230){
        cv::rectangle(image, cvPoint(theTrack.x+ 60, theTrack.y),
                      cvPoint(theTrack.x + + 30, theTrack.y + theTrack.height - 1), box_color, 2);
    }else if(frame <=249){
        cv::rectangle(image, cvPoint(theTrack.x + 60, theTrack.y),
                      cvPoint(theTrack.x + + 30, theTrack.y + theTrack.height - 1), box_color, 2);
    }else if(frame <=267){
        cv::rectangle(image, cvPoint(theTrack.x + 60, theTrack.y),
                      cvPoint(theTrack.x + 30, theTrack.y + 30), box_color, 2);
    }else{
        cv::rectangle(image, cvPoint(theTrack.x + 60, theTrack.y),
                      cvPoint(theTrack.x + 30, theTrack.y + 30), box_color, 2);
    }


    // Display the new image
    cv::imshow("Tracker Display", image);

    // Check if the user wants to exit early
    int key = cv::waitKey(100);
    if (key == 'q' || key == 'Q')
    {
      break;
    }
  }

  // Exit application
  std::cout << std::endl;
  return 0;
}


void ParkingApp::on_pushButton_clicked()
{
    myPlayer->loadImages("");
    myPlayer->Play();
    trackingDemo();
}

/***************************
 *
 *    recording the video into the pictures
 *
 ***************************/
void ParkingApp::on_Recording_clicked()
{
    cout<<"click the button" <<endl;
}


void ParkingApp::on_recordBtn_clicked(){
/*
    if(myPlayer->recording==false){
        myPlayer->recording = true;
        ui->recordBtn->setText(tr("recording"));
        myPlayer->recording_folder = ui->ipField->toPlainText().toLocal8Bit().data();
    }else{
        myPlayer->recording = false;
        ui->recordBtn->setText(tr("not recording"));
    }
*/
    recording = !recording;
    if(recording == 0){
        recording_index = 0;
        recording_folder = ui->ipField->toPlainText().toLocal8Bit().data();
        ui->recordBtn->setText(tr("Recording Start"));
    }else{
        ui->recordBtn->setText(tr("Recording Stop"));
        // automatically open the folder
    }
    cout << "recording is "<< recording <<endl;
}

void ParkingApp::on_record_type_btn_toggled(bool checked)
{
    if(checked) {

        /*
        string::size_type pAt = source.find_last_of('.');                  // Find extension point
        const string NAME = source.substr(0, pAt)  + ".avi";   // Form the new name with container
        int ex = static_cast<int>(inputVideo.get(CV_CAP_PROP_FOURCC));     // Get Codec Type- Int form
        VideoWriter outputVideo;
        Size S = Size((int) inputVideo.get(CV_CAP_PROP_FRAME_WIDTH),    //Acquire input size
                      (int) inputVideo.get(CV_CAP_PROP_FRAME_HEIGHT));
        outputVideo.open(NAME , ex, inputVideo.get(CV_CAP_PROP_FPS),S, true);
        */
        VideoWriter outputVideo("/home/helxsz/Desktop/capture.avi",CV_FOURCC('D','I','V','X'), 10, cvSize(640,480));

        if(!outputVideo.isOpened())
        {
            cout<<"Video writer not opening failed"<<endl;
            recording_type = 0;
        }else{
            cout<<"start to record the video success"<<endl;
            recording_type = 1;
        }
    }
    else {
        outputVideo.release();
        recording_type = 0;
    }
}


/******************************************************************
 *
 *
 *
 *
 ******************************************************************/
void ParkingApp::updateTime(){
    clock = clock.addSecs(2);
    //ui->label->setText(clock.toString("hh:mm:ss"));
    cout << "update time "<<endl;
    //myPlayer->monitor();
}

void ParkingApp::on_monitorBtn_clicked()
{
   //
   if( timer.isActive() )
   {
       timer.stop();
       ui->monitorBtn->setText(tr("Monitor start"));
   }
   else{
       timer.start();
       ui->monitorBtn->setText(tr("Monitor stop"));
   }
}

// http://creative-punch.net/2014/02/opening-displaying-saving-images-qt/
// http://blog.csdn.net/raodotcong/article/details/8785358  matlab and opencv

void ParkingApp::on_loadParkDataBtn_clicked()
{
       QString imagePath = QFileDialog::getOpenFileName(
                    this,
                    tr("Open File"),
                    "",
                    tr("JPEG (*.jpg *.jpeg);;PNG (*.png)" )
                    );
       cout << "output the image path : " <<imagePath.toUtf8().constData() << endl;
       if(!imagePath.isEmpty()){
            imageObject = new QImage();
            imageObject->load(imagePath);
            if(imageObject->isNull()){
                QMessageBox::information(this,"Image Viewer","Error Displaying image");
                return;
            }
            image = QPixmap::fromImage(*imageObject);
            scene = new QGraphicsScene(this);
            scene->addPixmap(image);
            scene->setSceneRect(image.rect());
            ui->graphicsView->setScene(scene);

            //////////////////////////////////////////////////
            Mat img = imread(imagePath.toUtf8().constData() );
            cout << img.size()<<endl;
            hogDetector->detectPerson(img,0);
       }

 /////////////////////////////////////////////////////////
      process = new QProcess(this);
      QStringList params;
      QString scriptFile = QCoreApplication::applicationDirPath() + "../../scriptPath/script.py";
      scriptFile = "/home/helxsz/helloworld.py";
      params =  QStringList() << scriptFile << "-f" << "1" << "-t" << "2" ;
      //printf("pycommand : %s \n", qStringToStdString(params.join(' ')).c_str());
      process->start("python",params);

      if(!process->waitForStarted()){
          cout << "process error "<<endl;
          qDebug() <<"shell error code "<< process->error();
          exit(1);
      }

      process->write("print hello \n");
      process->closeWriteChannel();

      connect(process, SIGNAL(readyRead()),this, SLOT(onProcessReadyRead()));


      process->waitForFinished(-1);
      QString p_stdout = process->readAll();
      cout << "execut the python script : " << QCoreApplication::applicationDirPath().toUtf8().constData() <<p_stdout.toUtf8().constData() << endl;


}

void ParkingApp::onProcessReadyRead(){
   QString p_stdout = process->readAll();
   qDebug() << p_stdout;
   qDebug() << "haha  "<<endl;
}

void ParkingApp::on_saveVideoBtn_clicked()
{
    QString imagePath = QFileDialog::getSaveFileName(
                this,
                tr("Save File"),
                "",
                tr("JPEG (*.jpg *.jpeg);;PNG (*.png)" )
                );

    *imageObject = image.toImage();
    imageObject->save(imagePath);
}


/********************
 *
 *   drag
 *
 ********************/
void ParkingApp::dragEnterEvent(QDragEnterEvent *e)
{
    if (e->mimeData()->hasUrls()) {
        foreach (const QUrl &url, e->mimeData()->urls()) {
            const QString &filename = url.toLocalFile();
            qDebug()<<filename;
            if(filename.endsWith("jpg")||filename.endsWith("png")||filename.endsWith("jpeg")){
               ui->camera->setStyleSheet("border: 1px dotted green");
               e->acceptProposedAction();
            }else if(filename.endsWith("avi") || filename.endsWith("mp4")){
               ui->camera->setStyleSheet("border: 1px dotted blue");
               e->acceptProposedAction();
            }else{
                ui->camera->setStyleSheet("border: 1px dotted red;background:white;");
            }
        }

    }

}

void ParkingApp::dragLeaveEvent(QDragEnterEvent *e){
    ui->camera->setStyleSheet("border: 1px dotted grey;background:white");

}

void ParkingApp::dropEvent(QDropEvent *e)
{
    QPoint p= e->pos();
    QPoint lp= ui->camera->pos();
    QRect r(lp.x(),lp.y(),ui->camera->width(),ui->camera->height());
    if(r.contains(p)){
        foreach (const QUrl &url, e->mimeData()->urls()) {
            const QString &fileName = url.toLocalFile();

            //ui->camera->setStyleSheet("border: 1px solid red");
            this->filename=fileName;
            emit fireDragSignal();

        }
    }
}

void ParkingApp::onfireDragSignal(){

    qDebug()<<"Process AVIfile";
    qDebug()<<filename;
    ui->ipField->setText(filename);

    //    ---------------------show the drag image-----------------------
    if(filename.endsWith("jpg")||filename.endsWith("png")||filename.endsWith("jpeg")){
        Mat image =imread(filename.toStdString().c_str());
        if(!image.empty()){

            imshow("image",image);
            waitKey(0);

        }
    }
    //    ---------------------show the drag video-----------------------
    else if(filename.endsWith("avi") || filename.endsWith("mp4")){
        if (!myPlayer->loadVideo(filename.toLatin1().data()))
        {
            QMessageBox msgBox;
            msgBox.setText("The selected video could not be opened!");
            msgBox.exec();
        }else{
            myPlayer->Play();
            ui->openBtn->setText(tr("Stop"));
        }
    }

    /*
    */
}
