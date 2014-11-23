#include "hogdetector.h"

/*
 *   http://docs.opencv.org/modules/objdetect/doc/latent_svm.html
 *   http://vision.caltech.edu/~sbranson/code/index.html
 *   http://www.juergenwiki.de/work/wiki/doku.php?id=public:deformable_parts_model_dpm
 *   http://blog.csdn.net/loadstar_kun/article/details/8686416
 *
 *  https://github.com/binuq8usa/BodyJointTracking
 *  https://github.com/binuq8usa/PersonReidentification/tree/master/Code
 *  https://github.com/liujiboyteam/LeafDetector/blob/master/HogSVM/HogSVM.cpp
 **/

HogDetector::HogDetector()
{
    //hog = HOGDescriptor(Size(32,32),Size(16,16),Size(8,8),Size(8,8),9);
    //hog = HOGDescriptor();
    hog.setSVMDetector(HOGDescriptor::getDefaultPeopleDetector());
}

HogDetector::~HogDetector(){

}

inline void show_histogram(std::string const& name, cv::Mat1b const& image)
{
    // Set histogram bins count
    int bins = 256;
    int histSize[] = {bins};
    // Set ranges for histogram bins
    float lranges[] = {0, 256};
    const float* ranges[] = {lranges};
    // create matrix for histogram
    cv::Mat hist;
    int channels[] = {0};

    // create matrix for histogram visualization
    int const hist_height = 256;
    cv::Mat3b hist_image = cv::Mat3b::zeros(hist_height, bins);

    cv::calcHist(&image, 1, channels, cv::Mat(), hist, 1, histSize, ranges, true, false);

    double max_val=0;
    minMaxLoc(hist, 0, &max_val);

    // visualize each bin
    for(int b = 0; b < bins; b++) {
        float const binVal = hist.at<float>(b);
        int   const height = cvRound(binVal*hist_height/max_val);
        cv::line
            ( hist_image
            , cv::Point(b, hist_height-height), cv::Point(b, hist_height)
            , cv::Scalar::all(255)
            );
    }
    cv::imshow(name, hist_image);
}

// http://www.magicandlove.com/blog/2011/08/26/people-detection-in-opencv-again/
void HogDetector::detectPerson(Mat &img, int index){
    cout << "detect the person "<<endl;
    vector<Rect> found, found_filtered;
    hog.detectMultiScale(img, found, 0, Size(8,8), Size(32,32), 1.05, 2);

    size_t i, j;
    for (i=0; i<found.size(); i++){
            Rect r = found[i];
            for (j=0; j<found.size(); j++)
                if (j!=i && (r & found[j])==r)
                    break;
            if (j==found.size())
                found_filtered.push_back(r);
    }

    for (i=0; i<found_filtered.size(); i++){
        Rect r = found_filtered[i];
            r.x += cvRound(r.width*0.1);
        r.width = cvRound(r.width*0.8);
        r.y += cvRound(r.height*0.06);
        r.height = cvRound(r.height*0.9);
        rectangle(img, r.tl(), r.br(), cv::Scalar(0,255,0), 2);
    }
    imshow("video capture", img);
}

// http://ccw1986.blogspot.co.uk/2013/08/opencvhogdescriptor.html
// http://www.juergenwiki.de/work/wiki/doku.php?id=public:hog_descriptor_computation_and_visualization
vector< float > HogDetector::findHogFeature(Mat &frame){
    cv::Mat src_gray;
    cv::cvtColor( frame, src_gray, CV_RGB2GRAY );
    cv::Mat src_resize;
    cv::resize(src_gray, src_resize, cvSize(60,60));
    /***************HOG Descriptor*****************/
    CvSize winSize = cvSize(60,60);
    CvSize blockSize = cvSize(20,20);
    CvSize strideSize = cvSize(10,10);
    CvSize cellSize = cvSize(5,5);
    int bins = 9;
    //一個Cell有9個方向，共9維
    //一個Block有16個Cells((20/5)*(20/5))，共144維
    //一個Window有25個Blocks(((60-20)/10+1)*((60-20)/10+1))，共3600維
    HOGDescriptor *hog= new  HOGDescriptor(winSize, blockSize, strideSize, cellSize, bins);
    vector< float >descriptors;//HOG描述向量
    CvSize winShiftSize = cvSize(10,10);//搜尋框移動位置
    CvSize paddingSize = cvSize(0,0);//補滿邊界位置
    hog->compute(src_resize, descriptors, winShiftSize, paddingSize);//調用計算函數
    printf( "%d\n" ,descriptors.size());

    cv::imshow( "result", src_resize );
    return descriptors;
}

void HogDetector::detectCar(Mat &frame, int index){
    cout << "hog detector detect car" << index <<endl;

}


// http://web.mit.edu/vondrick/ihog/
void HogDetector::visualize(Mat &frame){
    cout << "visualize the mat "<<endl;

}
