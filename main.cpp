#include "parkingapp.h"
#include <QApplication>

#include <opencv/cv.h>
#include <opencv/highgui.h>

#include <QDebug>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QUrl>
#include <QUrlQuery>

#include <stdio.h>
#include <gsl/gsl_sf_gamma.h>

//#include <boost/filesystem.hpp>
//#include <boost/iostreams/device/mapped_file.hpp>
#include <iostream>

// OpenCV Includes
#include <object_tracker.h>
#include <detector.h>


/*
#include "package_bgs/FrameDifferenceBGS.h"
#include "package_bgs/StaticFrameDifferenceBGS.h"
#include "package_bgs/WeightedMovingMeanBGS.h"
#include "package_bgs/WeightedMovingVarianceBGS.h"
#include "package_bgs/MixtureOfGaussianV1BGS.h"
#include "package_bgs/MixtureOfGaussianV2BGS.h"
#include "package_bgs/AdaptiveBackgroundLearning.h"
#include "package_bgs/AdaptiveSelectiveBackgroundLearning.h"

#if CV_MAJOR_VERSION >= 2 && CV_MINOR_VERSION >= 4 && CV_SUBMINOR_VERSION >= 3
#include "package_bgs/GMG.h"
#endif

#include "package_bgs/dp/DPAdaptiveMedianBGS.h"
#include "package_bgs/dp/DPGrimsonGMMBGS.h"
#include "package_bgs/dp/DPZivkovicAGMMBGS.h"
#include "package_bgs/dp/DPMeanBGS.h"
#include "package_bgs/dp/DPWrenGABGS.h"
#include "package_bgs/dp/DPPratiMediodBGS.h"
#include "package_bgs/dp/DPEigenbackgroundBGS.h"
#include "package_bgs/dp/DPTextureBGS.h"

#include "package_bgs/tb/T2FGMM_UM.h"
#include "package_bgs/tb/T2FGMM_UV.h"
#include "package_bgs/tb/T2FMRF_UM.h"
#include "package_bgs/tb/T2FMRF_UV.h"
#include "package_bgs/tb/FuzzySugenoIntegral.h"
#include "package_bgs/tb/FuzzyChoquetIntegral.h"

#include "package_bgs/lb/LBSimpleGaussian.h"
#include "package_bgs/lb/LBFuzzyGaussian.h"
#include "package_bgs/lb/LBMixtureOfGaussians.h"
#include "package_bgs/lb/LBAdaptiveSOM.h"
#include "package_bgs/lb/LBFuzzyAdaptiveSOM.h"

#if !defined(_WIN32)
// Currently this method works only on Linux platform.
#include "package_bgs/ck/LbpMrf.h"
#endif

#include "package_bgs/jmo/MultiLayerBGS.h"
// The PBAS algorithm was removed from BGSLibrary because it is
// based on patented algorithm ViBE
// http://www2.ulg.ac.be/telecom/research/vibe/
//#include "package_bgs/pt/PixelBasedAdaptiveSegmenter.h"
#include "package_bgs/av/VuMeter.h"
#include "package_bgs/ae/KDE.h"
#include "package_bgs/db/IndependentMultimodalBGS.h"
#include "package_bgs/sjn/SJN_MultiCueBGS.h"
#include "package_bgs/bl/SigmaDeltaBGS.h"
*/


//void test();
//int trackingDemo();
//int backgroundSubtrcationDemo();
//void sendRequest();
void loadConfig();
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);


    ParkingApp w;
    w.show();

    //Detector t;
    //t.init();
    //t.update();

     // http://stackoverflow.com/questions/23393870/read-write-file-with-unicode-file-name-with-plain-c-boost
     // boost::filesystem::path p(L"/home/helxsz/Desktop/documents.txt");
     // boost::iostreams::mapped_file file(p); // or mapped_file_source
     // std::cout << file.data() << std::endl;

    //loadConfig();
    //sendRequest();
    //test();
    //backgroundSubtrcationDemo();
    //trackingDemo();
    return a.exec();
}


cv::Mat mkKernel(int ks, double sig, double th, double lm, double ps)
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

void on_mouse( int event, int x, int y, int d, void *ptr )
{
     if  ( event == EVENT_LBUTTONDOWN )
     {
          cout << "Left button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
     }
     else if  ( event == EVENT_RBUTTONDOWN )
     {
          cout << "Right button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
     }
     else if  ( event == EVENT_MBUTTONDOWN )
     {
          cout << "Middle button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
     }
     else if ( event == EVENT_MOUSEMOVE )
     {
          cout << "Mouse move over the window - position (" << x << ", " << y << ")" << endl;

     }
    }

void test(){
    cout << "what the fuck " <<endl;
    double a1 = 20;
    double b = 1000;
    double x = 0.5;
    double result = gsl_sf_beta_inc(a1, b, x);
    cout << result << endl;

    cv::Mat in = cv::imread("/home/helxsz/Desktop/test/test2/1.jpg");          // load grayscale
    cv::Mat dest;
    cv::Mat src_f;
    cv::cvtColor(in, in, CV_BGR2GRAY);
    //imshow("d11",in);
    //in.convertTo(src_f,CV_32F);
    in.convertTo(src_f, CV_32F, 1.0/255, 0);
    namedWindow( "reference", WINDOW_AUTOSIZE );
    setMouseCallback("reference",on_mouse, NULL );
    imshow("reference",src_f);


    int kernel_size = 31;
    double sig = 1, th = 0, lm = 1.0, gm = 0.02, ps = 0;
    cv::Mat kernel = cv::getGaborKernel(cv::Size(kernel_size,kernel_size), sig, th, lm, gm, ps);
    cv::filter2D(src_f, dest, CV_32F, kernel);

    //cerr << dest(Rect(30,30,10,10)) << endl; // peek into the data
    //imshow("d",dest);


    ///////////////////////////////////////////////////////////////////
    int pos_sigma= 5;
    int pos_lm = 50;
    int pos_th = 0;
    int pos_psi = 90;


        sig = pos_sigma;
        lm = 0.5+pos_lm/100.0;
        th = pos_th;
        ps = pos_psi;
       kernel = mkKernel(kernel_size, sig, th, lm, ps);
       cv::filter2D(src_f, dest, CV_32F, kernel);
       cv::imshow("Process window", dest);
       /**/




}




void loadConfig()
{
    cout << "load config 1111111 "<<endl;
  CvFileStorage* fs = cvOpenFileStorage("FrameDifferenceBGS.xml", 0, CV_STORAGE_READ);

  bool enableThreshold = cvReadIntByName(fs, 0, "enableThreshold", true);
  int threshold = cvReadIntByName(fs, 0, "threshold", 151);
  bool showOutput = cvReadIntByName(fs, 0, "showOutput", true);
  cout << threshold<< "    "<<showOutput<<"    "<<  enableThreshold<<endl;
  cvReleaseFileStorage(&fs);

/*

  //fs = cvOpenFileStorage("/home/helxsz/workspace/qt/particle_filter/config/FrameDifferenceBGS.xml", 0, CV_STORAGE_WRITE);
  //fs = cvOpenFileStorage("/home/helxsz/workspace/qt/particle_filter/config/FrameDifferenceBGS.xml", 0, CV_STORAGE_WRITE);

  fs = cvOpenFileStorage("./config/FrameDifferenceBGS.xml", 0, CV_STORAGE_WRITE);


  cvWriteInt(fs, "enableThreshold", 21);
  cvWriteInt(fs, "threshold", true);
  //cvWriteInt(fs, "showOutput", true);

  cvReleaseFileStorage(&fs);
  */
}

int trackingDemo()
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
  //const char* directory = "/home/helxsz/workspace/data/david";
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



/*
int backgroundSubtrcationDemo(){

    std::cout << "Using OpenCV " << CV_MAJOR_VERSION << "." << CV_MINOR_VERSION << "." << CV_SUBMINOR_VERSION << std::endl;

    // Background Subtraction Methods
    //IBGS *bgs;

    //bgs = new FrameDifferenceBGS();
    //bgs = new StaticFrameDifferenceBGS;
    //bgs = new WeightedMovingMeanBGS;
    //bgs = new WeightedMovingVarianceBGS;
    //bgs = new MixtureOfGaussianV1BGS;
    //bgs = new MixtureOfGaussianV2BGS;
    //bgs = new AdaptiveBackgroundLearning;
    //bgs = new AdaptiveSelectiveBackgroundLearning;
    //bgs = new GMG;

    // DP Package (thanks to Donovan Parks)
    //bgs = new DPAdaptiveMedianBGS;
    //bgs = new DPGrimsonGMMBGS;
    //bgs = new DPZivkovicAGMMBGS;   // ok
    //bgs = new DPMeanBGS;
    bgs = new DPWrenGABGS;   // ok
    //bgs = new DPPratiMediodBGS; // bad
    // bgs = new DPEigenbackgroundBGS;  // too many noice
    //bgs = new DPTextureBGS;   // fucking slow

    // TB Package (thanks to Thierry Bouwmans, Fida EL BAF and Zhenjie Zhao)
    //bgs = new T2FGMM_UM;  // dots
    //bgs = new T2FGMM_UV;    // too many noise
    //bgs = new T2FMRF_UM;
    //bgs = new T2FMRF_UV;
    //bgs = new FuzzySugenoIntegral;
    //bgs = new FuzzyChoquetIntegral;

    // JMO Package (thanks to Jean-Marc Odobez)
    //bgs = new MultiLayerBGS;

    // PT Package (thanks to Martin Hofmann, Philipp Tiefenbacher and Gerhard Rigoll)
    //bgs = new PixelBasedAdaptiveSegmenter;

    // LB Package (thanks to Laurence Bender)
    //bgs = new LBSimpleGaussian;
    //bgs = new LBFuzzyGaussian;
    //bgs = new LBMixtureOfGaussians;
    //bgs = new LBAdaptiveSOM;
    //bgs = new LBFuzzyAdaptiveSOM;

    // LBP-MRF Package (thanks to Csaba Kertész)
    //bgs = new LbpMrf;

    // AV Package (thanks to Lionel Robinault and Antoine Vacavant)
    //bgs = new VuMeter;

    // EG Package (thanks to Ahmed Elgammal)
    //bgs = new KDE;

    // DB Package (thanks to Domenico Daniele Bloisi)
    //bgs = new IndependentMultimodalBGS;

    // SJN Package (thanks to SeungJong Noh)
    //bgs = new SJN_MultiCueBGS;

    // BL Package (thanks to Benjamin Laugraud)
    //bgs = new SigmaDeltaBGS;

    int frameNumber = 1;
    int key = 0;
    while(key != 'q')
    {
      std::stringstream ss;
      ss << frameNumber;
      //std::string fileName = "/opt/bgslibrary/frames/" + ss.str() + ".png";
      std::string fileName = "/home/helxsz/Desktop/test/test1/" + ss.str() + ".jpg";
      std::cout << "reading " << fileName << std::endl;

      cv::Mat img_input = cv::imread(fileName, CV_LOAD_IMAGE_COLOR);

      if (img_input.empty())
        break;

      cv::imshow("input", img_input);

      cv::Mat img_mask;
      cv::Mat img_bkgmodel;
      bgs->process(img_input, img_mask, img_bkgmodel); // by default, it shows automatically the foreground mask image






      //if(!img_mask.empty())
      //  cv::imshow("Foreground", img_mask);
      //  do something

      key = cvWaitKey(33);
      frameNumber++;
    }
    cvWaitKey(0);
    delete bgs;

    cvDestroyAllWindows();
    return 0;
}
*/





void sendRequest(){

    // create custom temporary event loop on stack
    QEventLoop eventLoop;

    // "quit()" the event-loop, when the network request "finished()"
    QNetworkAccessManager mgr;
    QObject::connect(&mgr, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));

    // the HTTP request
    QNetworkRequest req( QUrl( QString("http://ip.jsontest.com/") ) );
    QNetworkReply *reply = mgr.get(req);
    eventLoop.exec(); // blocks stack until "finished()" has been called

    if (reply->error() == QNetworkReply::NoError) {
        //success
        qDebug() << "Success" <<reply->readAll();
        delete reply;
    }
    else {
        //failure
        qDebug() << "Failure" <<reply->errorString();
        delete reply;
    }

    /*
        QNetworkAccessManager *manager = new QNetworkAccessManager(this);

        QUrl url("https://accounts.google.com/o/oauth2/token");
        QNetworkRequest request(url);

        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

        QUrl params;
        params.addQueryItem("client_id", "...");
        params.addQueryItem("client_secret", "...");
        params.addQueryItem("code", "...");
        // etc

        QObject::connect(manager, SIGNAL(finished(QNetworkReply *)), this, SLOT(replyFinished(QNetworkReply *)));

        manager->post(request, params.encodedQuery());

     */
}
