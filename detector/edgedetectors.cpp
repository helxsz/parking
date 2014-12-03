#include "edgedetectors.h"
using namespace std;

    edgeDetectors::edgeDetectors()
    {
    }

    edgeDetectors::~edgeDetectors()
    {
        cout<< "edge detector ...."<<endl;

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

    void edgeDetectors::detectCar(Mat frame, int index){

        cout << "detect the car ....  "<<endl;
        Mat gray;
        cv::cvtColor(frame, gray, CV_BGR2GRAY);
        Mat src_f, dest;
        gray.convertTo(src_f, CV_32F, 1.0/255, 0);
        int kernel_size = 31;
        double sig = 1, th = 0, lm = 1.0, gm = 0.02, ps = 0;
        int pos_sigma= 5;
        int pos_lm = 50;
        int pos_th = 0;
        int pos_psi = 90;
        sig = pos_sigma;
        lm = 0.5+pos_lm/100.0;
        th = pos_th;
        ps = pos_psi;
        cv::Mat kernel = mkKernel(kernel_size, sig, th, lm, ps);
        cv::filter2D(src_f, dest, CV_32F, kernel);
        //cv::imshow("Process window", dest);

        //cv::circle(  frame, Point(10,10), 15,  Scalar(0,100,0), 2, 8, 0  );

        // http://opencvexamples.blogspot.com/2013/10/void-canny-inputarray-image-outputarray.html#.VGTX_t_mgUQ
        /*
        Mat edge, draw;
        Canny( gray, edge, 50, 150, 3);
        edge.convertTo(draw, CV_8U);
        */
        // http://opencvexamples.blogspot.com/2013/10/edge-detection-using-laplacian-operator.html#.VGTX_9_mgUQ
        /*
        Mat dst,abs_dst;
        Laplacian( gray, dst, CV_16S, 3, 1, 0, BORDER_DEFAULT );
        convertScaleAbs( dst, abs_dst );
        */
        // http://opencvexamples.blogspot.com/2013/10/sobel-edge-detection.html#.VGTYAt_mgUQ
        /*
        Mat sobelx;
        Sobel(grey, sobelx, CV_32F, 1, 0);

        double minVal, maxVal;
        minMaxLoc(sobelx, &minVal, &maxVal); //find minimum and maximum intensities
        cout << "minVal : " << minVal << endl << "maxVal : " << maxVal << endl;

        Mat draw;
        sobelx.convertTo(draw, CV_8U, 255.0/(maxVal - minVal), -minVal * 255.0/(maxVal - minVal));
        */
        //detector.detect(frame, getCurrentFrame());
        // http://stackoverflow.com/questions/12335663/getting-enum-names-e-g-cv-32fc1-of-opencv-image-types
        //QImage q = cvMatToQImage(dest);
        ////
    }




