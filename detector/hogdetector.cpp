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

// http://www.bogotobogo.com/Qt/Qt5_QThreads_GuiThread.php
HogDetector::HogDetector(QObject *parent):QThread(parent)
{
    hog = new HOGDescriptor();


    params.svm_type = CvSVM::C_SVC;
    params.kernel_type = CvSVM::LINEAR;
    params.term_crit = cvTermCriteria(CV_TERMCRIT_ITER, 100, 1e-6);

    svm = new CvSVM();
}

HogDetector::~HogDetector(){

}

inline void get_svm_detector(const Ptr<SVM>& svm, vector< float > & hog_detector )
{
    /*
    // get the support vectors
    Mat sv = svm->getSupportVectors();


    const int sv_total = sv.rows;
    // get the decision function
    Mat alpha, svidx;
    double rho = svm->getDecisionFunction(0, alpha, svidx);

    CV_Assert( alpha.total() == 1 && svidx.total() == 1 && sv_total == 1 );
    CV_Assert( (alpha.type() == CV_64F && alpha.at<double>(0) == 1.) ||
               (alpha.type() == CV_32F && alpha.at<float>(0) == 1.f) );
    CV_Assert( sv.type() == CV_32F );
    hog_detector.clear();

    hog_detector.resize(sv.cols + 1);
    memcpy(&hog_detector[0], sv.ptr(), sv.cols*sizeof(hog_detector[0]));
    hog_detector[sv.cols] = (float)-rho;
*/
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
    hog = new HOGDescriptor();
    hog->setSVMDetector(HOGDescriptor::getDefaultPeopleDetector());
    hog->detectMultiScale(img, found, 0, Size(8,8), Size(32,32), 1.05, 2);

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

/*
inline void get_primal_form_support_vector(std::vector<float>& support_vector) const {
    int sv_count = get_support_vector_count();
    const CvSVMDecisionFunc* df = decision_func;
    const double* alphas = df[0].alpha;
    double rho = df[0].rho;
    int var_count = get_var_count();
    support_vector.resize(var_count, 0);
    for (unsigned int r = 0; r < (unsigned)sv_count; r++) {
      float myalpha = alphas[r];
      const float* v = get_support_vector(r);
      for (int j = 0; j < var_count; j++,v++) {
        support_vector[j] += (-myalpha) * (*v);
      }
    }
    support_vector.push_back(rho);
}
*/

void HogDetector::detectCar(Mat &img, int index){
    cout << "hog detector detect car" << index <<endl;
    vector<Rect> found, found_filtered;


    win_size = Size(80,40);
    block_size = Size(20,10);
    block_stride = Size(10,5);
    cell_size = Size(5,5);
    int bins = 9;
    hog = new HOGDescriptor( win_size , block_size , block_stride , cell_size , 9);
    svm->load("/home/helxsz/Desktop/car.xml");

    // Set the trained svm to my_hog
        vector< float > hog_detector;
        get_svm_detector( svm, hog_detector );
        hog->setSVMDetector( hog_detector );

    // http://blog.csdn.net/masibuaa/article/details/16105073
    // http://stackoverflow.com/questions/25158984/getting-primal-form-from-cvsvm-trained-file
    // http://stackoverflow.com/questions/26806066/how-to-calculate-confidence-score-from-cvsvm
    /*
    int DescriptorDim = svm->get_var_count();//特征向量的维数，即HOG描述子的维数
    int supportVectorNum = svm->get_support_vector_count();//支持向量的个数
    cout<<"支持向量个数："<<supportVectorNum<<endl;

    Mat alphaMat = Mat::zeros(1, supportVectorNum, CV_32FC1);//alpha向量，长度等于支持向量个数
    Mat supportVectorMat = Mat::zeros(supportVectorNum, DescriptorDim, CV_32FC1);//支持向量矩阵
    Mat resultMat = Mat::zeros(1, DescriptorDim, CV_32FC1);//alpha向量乘以支持向量矩阵的结果

    //将支持向量的数据复制到supportVectorMat矩阵中
    for(int i=0; i<supportVectorNum; i++)
    {
        const float * pSVData = svm->get_support_vector(i);//返回第i个支持向量的数据指针
        for(int j=0; j<DescriptorDim; j++)
        {
            //cout<<pData[j]<<" ";
            supportVectorMat.at<float>(i,j) = pSVData[j];
        }
    }

    //将alpha向量的数据复制到alphaMat中


    const CvSVMDecisionFunc* df = svm->decision_func;
    double * pAlphaData =  svm->decision_func->alpha;  //svm->get_alpha_vector();//返回SVM的决策函数中的alpha向量
    for(int i=0; i<supportVectorNum; i++)
    {
        alphaMat.at<float>(0,i) = pAlphaData[i];
    }

    //计算-(alphaMat * supportVectorMat),结果放到resultMat中
    //gemm(alphaMat, supportVectorMat, -1, 0, 1, resultMat);//不知道为什么加负号？
    resultMat = -1 * alphaMat * supportVectorMat;

    //得到最终的setSVMDetector(const vector<float>& detector)参数中可用的检测子
    vector<float> myDetector;
    //将resultMat中的数据复制到数组myDetector中
    for(int i=0; i<DescriptorDim; i++)
    {
        myDetector.push_back(resultMat.at<float>(0,i));
    }
    //最后添加偏移量rho，得到检测子
    myDetector.push_back(svm->decision_func->rho);
    cout<<"检测子维数："<<myDetector.size()<<endl;
    hog->setSVMDetector(myDetector);
    */

/////////////////////////////////////////////////////////////////////////////////

    //hog->setSVMDetector(HOGDescriptor::getDefaultPeopleDetector());
    // https://github.com/miaowangtw/OpenCV2.4.9_CB/blob/7754bac345b7a5e2b90a24b0d90b79a58d234d71/sources/modules/ocl/test/test_objdetect.cpp
    Mat hogFeature(1,hog->getDescriptorSize(),CV_32FC1);

    if(!img.empty()) {
        imshow("test image",img);
        cout << "   "<<   img.cols <<"     "<<img.rows<< "   "  <<endl;
        computeHOG(img,hogFeature);
        float car = svm->predict(hogFeature,true);
        cout << "get predict  "<<car <<endl;


        // https://github.com/qjsmile/bysj1/blob/e6e8965bc255aa736aefc222551bf7f4721f05c4/bysj/test7/main.cpp
        /*

        hog->detectMultiScale(img, found, 0, Size(8,8), Size(32,32), 1.05, 2);

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
        */


    }



}


// http://web.mit.edu/vondrick/ihog/
void HogDetector::visualize(Mat &frame){
    cout << "visualize the mat "<<endl;

}


void HogDetector::computeHOG(Mat &image, Mat &hogFeature)
{
    //Mat *im = &Mat();
    Mat im;
    vector<float> desc;

    if(image.rows != win_size.height || image.cols != win_size.width){
        //resize(image,*im,win_size);
        resize(image,im,win_size);
        cout << "resize the picture  " <<endl;
    }else
        //im = &image;
        im = image.clone();

    hog->compute(im,desc);
    for(int i =0; i < desc.size(); i++)
    {
        hogFeature.at<float>(0,i) = desc.at(i);
    }

}


void HogDetector::train(){

    /*          */
    QString path = "/home/helxsz/Desktop/CarData/TrainImages/"; // QDir::currentPath() + "/" + QString path = QDir::currentPath() + "/"+ QString::fromNumber(year) + "/" + QString::fromNumber(month) + "/" + QString::fromNumber(day);
    QDir dir(path);
    QStringList files = dir.entryList();

    win_size = Size(80,40);
    block_size = Size(20,10);
    block_stride = Size(10,5);
    cell_size = Size(5,5);
    int bins = 9;
    hog = new HOGDescriptor( win_size , block_size , block_stride , cell_size , 9);

   try{
        Mat training_data_mat(0,hog->getDescriptorSize(),CV_32FC1);
        Mat labels_mat(0,1,CV_32FC1);
        Mat hogFeature(1,hog->getDescriptorSize(),CV_32FC1);

        //
        foreach(QFileInfo finfo, files) {
            //qDebug() << path + finfo.fileName() << endl;
            //qDebug() << (path + finfo.fileName()).toLatin1().data() << endl;
            /*
            if (finfo.isDir()) {
                    listFiles(QDir(finfo.absoluteFilePath()), indent);
            }
            */
            // https://github.com/shengbinmeng/CarDetector/blob/master/SVMGenerator/GenerateSVM.cpp

            Mat img = imread(  (path + finfo.fileName()).toLatin1().data());
            float label = 0.0;
            if( finfo.fileName().startsWith("pos") ){
                label =1.0 ;
            }else if(finfo.fileName().startsWith("neg")){
                label = -1.0;
            }

            if(!img.empty()) { imshow("test image",img);
                cout << (path + finfo.fileName()).toLatin1().data()<< "   "<<   img.cols <<"     "<<img.rows<< "   " << label <<endl;
               computeHOG(img,hogFeature);

               training_data_mat.push_back(hogFeature);
               labels_mat.push_back(label);

            }else{
                cout << "img not here "<<endl;
            }
            //
        }

        cout << "training starts  "<< training_data_mat.rows<<"     "<<training_data_mat.cols<<"    "<< labels_mat.rows <<"   "<<labels_mat.cols<<endl;
        svm->train(training_data_mat, labels_mat, Mat(), Mat(), params);

        // Save vehicleDetector SVM
        svm->save("/home/helxsz/Desktop/car.xml");

        cout << "Training Completed!" << endl;
        //cout << "Vehicle Detector model saved to: " << endl;

    }catch(const exception& ex)
    {
        cerr << "Error: " << ex.what() << endl;
        cin.get(); //keep_terminal_open();
        exit(0);
    }

}



// run() will be called when a thread starts
void HogDetector::run()
{

    clock.start();
    train();
    int nMilliseconds = clock.elapsed();
    printf("Descriptors Generate Time:%f milli seconds.\n",nMilliseconds);


    for(int i = 0; i <= 1; i++)
    {
        QMutex mutex;
        // prevent other threads from changing the "Stop" value
        mutex.lock();
        if(this->stop) break;
        mutex.unlock();
        //cout << i <<endl;
        // emit the signal for the count label
        emit valueChanged(i);
        // slowdown the count change, msec
        this->msleep(200);
    }
    //emit trainingValueChanged(100);
}
