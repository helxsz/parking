#include "detector.h"
using namespace std;

    Detector::Detector()
    {
        cout<< "detector ...."<<endl;

    }

    Detector::~Detector()
    {
        cout<< "detector ...."<<endl;

    }

    void Detector::init(){
       cout<< "init ...."<<endl;

    }

    //Stop the video
    void Detector::detect(Mat &frame ,int num){

        //imshow("abc",frame);
        //cv::cvtColor(frame, RGBframe, CV_BGR2RGB);

       cout<< "detect ...."<<num <<endl;

       cv::Mat back;
       cv::Mat fore;
       //bg.nmixtures = 3;bg.bShadowDetection = false;
       //bg.set("bShadowDetection", false);
       //bg.set("nmixtures", 3);
       // how to set the parameters
       //http://answers.opencv.org/question/18868/ideal-motionobject-tracking-method-problems-with/

       std::vector<std::vector<cv::Point> > contours;

       cv::namedWindow("Background");

       Mat edges;
       bg.operator ()(frame,fore,-0.7);
       bg.getBackgroundImage(back);

       cvtColor(frame,frame,CV_RGB2GRAY);
       equalizeHist( frame, frame );
       threshold(fore,fore,128,255,THRESH_BINARY);
       medianBlur(fore,fore,17);                        // can't detect the human movement with 9, but ok with 5. 5 with noizy
       //GaussianBlur(fore,fore,Size(7,7),1.5,1.5,4);    // many noises on the screen

       //The mask can be quite noisy, so in order to get rid of the noise we do an opening.
       cv::erode(fore,fore,cv::Mat());
       cv::dilate(fore,fore,cv::Mat());

       //After detection it would be nice to detect and draw some contours. We use two functions to do this:

       //inRange(back, Scalar(100, 100, 100), Scalar(120, 255, 255), back);

       cv::findContours(fore,contours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);
       RNG rng(12345);
       /// Approximate contours to polygons + get bounding rects and circles
       // http://docs.opencv.org/doc/tutorials/imgproc/shapedescriptors/bounding_rects_circles/bounding_rects_circles.html
       vector<vector<Point> > contours_poly( contours.size() );
       vector<Rect> boundRect( contours.size() );
       vector<Point2f>center( contours.size() );
       vector<float>radius( contours.size() );

       for( int i = 0; i < contours.size(); i++ )
       {
           approxPolyDP( Mat(contours[i]), contours_poly[i], 3, true );
           boundRect[i] = boundingRect( Mat(contours_poly[i]) );
           minEnclosingCircle( (Mat)contours_poly[i], center[i], radius[i] );
       }

       //cout<< contours.size() << endl;
       for(int i = 0;i <contours.size();i++){
           cout << "contour id:"<<i<< contours[i].size() << " length:  "<< arcLength(contours[i],false) <<"area:" << contourArea(contours[i],false) << endl;
            if(contours[i].size()>40){
            //if(boundRect[i].area() <min_size || boundRect[i].area() >max_size){
               Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
               cv::drawContours(frame,contours,i,cv::Scalar(0,0,155),1);
               rectangle( frame, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0 );
              // circle( frame, center[i], (int)radius[i], color, 2, 8, 0 );
           }
       }
       // get contours size, get contour shape , get contour center, get contour area

       // segement the contour out of background

       // tracker this as a object

       // trace the object with line , location


       //cv::drawContours(frame,contours,-1,cv::Scalar(0,0,255),2);
       //rectangle( frame, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0 );
       if(num % (10* 20) == 0 ){
           //destroyWindow("Frame");
           //cv::namedWindow("Frame");
           //cv::imshow("Frame",frame);
       }else{
           //cv::imshow("Frame",frame);
       }
       cv::imshow("Frame",frame);
       // cv::imshow("Background",back);
       // cv::imshow("fore",fore);
       // if(waitKey(30) >= 0) break;
       // waitKey(30);
   }


      //destroyWindow("Frame");
      //destroyWindow("Background");
      // how to release the video memory

    void Detector::update(){
       cout<< "update ...."<<endl;

    }


    void Detector::detect2(Mat &frame ,int num){

        //imshow("abc",frame);
        //cv::cvtColor(frame, RGBframe, CV_BGR2RGB);

       cout<< "detect ...."<<num <<endl;

       cv::Mat back;
       cv::Mat fore;
       //bg.nmixtures = 3;bg.bShadowDetection = false;
       //bg.set("bShadowDetection", false);
       //bg.set("nmixtures", 3);
       // how to set the parameters
       //http://answers.opencv.org/question/18868/ideal-motionobject-tracking-method-problems-with/

       std::vector<std::vector<cv::Point> > contours;

       //cv::namedWindow("Background");

       Mat edges;
       bg.operator ()(frame,fore,-0.7);
       bg.getBackgroundImage(back);

       cvtColor(frame,frame,CV_RGB2GRAY);
       equalizeHist( frame, frame );
       threshold(fore,fore,128,255,THRESH_BINARY);
       medianBlur(fore,fore,17);                        // can't detect the human movement with 9, but ok with 5. 5 with noizy
       //GaussianBlur(fore,fore,Size(7,7),1.5,1.5,4);    // many noises on the screen

       //The mask can be quite noisy, so in order to get rid of the noise we do an opening.
       cv::erode(fore,fore,cv::Mat());
       cv::dilate(fore,fore,cv::Mat());



    }


/*
    #define min_size 500
    #define max_size 10000
    void Detector::DIFF2(char *path) {

        VideoCapture cap(path);
        Mat fore;

        namedWindow("fore",CV_WINDOW_AUTOSIZE);
        namedWindow("original",CV_WINDOW_AUTOSIZE);

        if(!cap.isOpened()){
            cout << "reading from video is wrong" <<endl;
            exit(0);
        }

        cout << cap.get(CV_CAP_PROP_FRAME_COUNT) << cap.get(CV_CAP_PROP_FRAME_HEIGHT) << cap.get(CV_CAP_PROP_FRAME_WIDTH) <<endl;
        std::vector<std::vector<cv::Point> > contours;
        while(1){
            Mat frame1, frame2,frame3;
            cap.read(frame1);
            cvtColor(frame1,frame1,CV_RGB2GRAY);
            imshow("original",frame1);

            cap.read(frame2);
            cvtColor(frame2,frame2,CV_RGB2GRAY);
            imshow("original",frame2);

            cap.read(frame3);
            cvtColor(frame3,frame3,CV_RGB2GRAY);
            imshow("original",frame3);

            if(frame1.empty() ||frame2.empty() ||frame3.empty() ) break;

            Mat fore1,fore2;
            subtract(frame2,frame1,fore1);
            subtract(frame3,frame2,fore2);
            bitwise_or(fore1,fore2,fore);

            //threshold(fore,fore,128,255,THRESH_BINARY);
            medianBlur(fore,fore,3);
            //The mask can be quite noisy, so in order to get rid of the noise we do an opening.
            cv::erode(fore,fore,cv::Mat());
            cv::dilate(fore,fore,cv::Mat());

            for(int i=0;i<fore.rows;i++)
                for(int j =0;j<fore.cols;j++){
                    if(fore.at<unsigned char>(i,j) > 20){
                        fore.at<unsigned char>(i,j)  = 255;
                    }else
                        fore.at<unsigned char>(i,j) = 0;
            }

            imshow("fore",fore);

            //////////////////////////////////////////////////////////////////////////////////////////
            cv::findContours(fore,contours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);

            RNG rng(12345);

            /// Approximate contours to polygons + get bounding rects and circles
            // http://docs.opencv.org/doc/tutorials/imgproc/shapedescriptors/bounding_rects_circles/bounding_rects_circles.html
            vector<vector<Point> > contours_poly( contours.size() );
            vector<Rect> boundRect( contours.size() );
            vector<Point2f>center( contours.size() );
            vector<float>radius( contours.size() );

            for( int i = 0; i < contours.size(); i++ )
            {
                approxPolyDP( Mat(contours[i]), contours_poly[i], 3, true );
                boundRect[i] = boundingRect( Mat(contours_poly[i]) );
                minEnclosingCircle( (Mat)contours_poly[i], center[i], radius[i] );
            }

            //cout<< contours.size() << endl;
            for(int i = 0;i <contours.size();i++){
                cout << "contour id:"<<i<< contours[i].size() << " length:  "<< arcLength(contours[i],false) <<"area:" << contourArea(contours[i],false) << endl;
                 if(contours[i].size()>40){
                 //if(boundRect[i].area() <min_size || boundRect[i].area() >max_size){
                    Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
                    cv::drawContours(frame1,contours,i,cv::Scalar(0,0,155),1);
                    rectangle( frame1, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0 );
                   // circle( frame, center[i], (int)radius[i], color, 2, 8, 0 );
                }
            }
            imshow("original",frame1);
            waitKey(150);
        }//while
        cap.release();

    }


    void  Detector::GMM(char *path){

    }

*/
