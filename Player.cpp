#include <player.h>

using namespace std;

Player::Player(QObject *parent): QThread(parent)
{
   stop = true;
}

bool Player::loadVideo(string filename) {
   type = 0;
   capture  =  new cv::VideoCapture(filename);
   if (capture->isOpened())
   {
       frameRate = (int) capture->get(CV_CAP_PROP_FPS);
       return true;
   }
   else
   return false;
}

/*
bool Player::Recording(string filename){
   type = 0;
   capture  =  new cv::VideoCapture(filename);
   if (capture->isOpened())
   {
       recording = true;
       frameRate = (int) capture->get(CV_CAP_PROP_FPS);
       return true;
   }
   else
   return false;
}
*/


bool Player::loadImages(string filename) {
    type  = 1;
    currentImageIndex = 1;
}

void Player::Play()
{
    if (!isRunning()) {
       if (isStopped()){
           stop = false;
        }
       start(LowPriority);
    }
}



Player::~Player()
{
    mutex.lock();
    stop = true;
    //capture.release();
    delete capture;
    condition.wakeOne();
    mutex.unlock();
    wait();
}
void Player::Stop()
{
    stop = true;
}

void Player::msleep(int ms){
    struct timespec ts = { ms / 1000, (ms % 1000) * 1000 * 1000 };
    nanosleep(&ts, NULL);
}

bool Player::isStopped() const{
    return this->stop;
}

void Player::run()
{
   int delay = (1000/frameRate);
   if(type == 0){
       while(!stop){

          if (!capture->read(frame))
          {
              stop = true;
          }
/*
          if (frame.channels()== 3){
              cv::cvtColor(frame, RGBframe, CV_BGR2RGB);

              img = QImage((const unsigned char*)(RGBframe.data),
              RGBframe.cols,RGBframe.rows,QImage::Format_RGB888);
              img.bits();
          }
          else{
              img = QImage((const unsigned char*)(frame.data),frame.cols,frame.rows,QImage::Format_Indexed8);
          }
          emit processedImage(img, getCurrentFrame() );
*/
          emit processImageMat(frame, getCurrentFrame() );
          emit testData(   getCurrentFrame()  , getNumberOfFrames()   );

          this->msleep(delay);
       }
   }else if(type ==1){

       cout << "type ==1 "<<endl;
       int start = 1;
       int stop = 462;
       int delta = 1;
       char* prefix = "img";
       char* suffix = "png";
       char filename[1024];
       char* directory = "/home/helxsz/Desktop/test/test1/";

      while(currentImageIndex>=start && currentImageIndex <=462){
          currentImageIndex ++;
          sprintf(filename, "%s/%s%05d.%s", directory, prefix, currentImageIndex, suffix);
          char integer_string[8];
          sprintf(integer_string, "%d", currentImageIndex);
          char path[64] = "/home/helxsz/Desktop/test/test1/";
          strcat(path, integer_string);
          strcat(path , ".jpg");
          //cout<< currentImageIndex <<endl;

          this->msleep(260);

          frame = cv::imread(path);
          if (frame.empty())
          {
            std::cerr << "Error loading image file: \n" << std::endl;
            break;
          }
          emit processImageMat(frame, currentImageIndex);

          /*
          if (frame.channels()== 3){
              cv::cvtColor(frame, RGBframe, CV_BGR2RGB);

              img = QImage((const unsigned char*)(RGBframe.data),
              RGBframe.cols,RGBframe.rows,QImage::Format_RGB888);
              img.bits();

          }
          else{
              img = QImage((const unsigned char*)(frame.data),frame.cols,frame.rows,QImage::Format_Indexed8);
          }
          emit processedImage(img, currentImageIndex);
  */
      }
      cout<< "endl" <<endl;
   }

}

double Player::getCurrentFrame(){
    return capture->get(CV_CAP_PROP_POS_FRAMES);
}

double Player::getNumberOfFrames(){
    return capture->get(CV_CAP_PROP_FRAME_COUNT);
}

double Player::getFrameRate(){
    return frameRate;
}

void Player::setCurrentFrame( int frameNumber ){
    capture->set(CV_CAP_PROP_POS_FRAMES, frameNumber);
}
