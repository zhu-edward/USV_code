#include <iostream> // for standard I/O
#include <string>   // for strings

#include <opencv2/core/core.hpp>        // Basic OpenCV structures (cv::Mat)
#include <opencv2/highgui/highgui.hpp>  // Video write

//g++ -o recordVideo recordVideo.cpp `pkg-config --cflags --libs opencv`
using namespace std;
using namespace cv;

int main(int argc, char* argv[])
{
    if (argc < 2){
        cout << "Usage: ./recordVideo <nameOfVideo>.avi \nesc Key to exit\n";
        return 0;
    }

    VideoCapture cap(1); // open the video camera no. 0. 1 for own webcam

    cap.set(CV_CAP_PROP_FRAME_WIDTH, 1280);
    cap.set(CV_CAP_PROP_FRAME_HEIGHT, 720);

    if (!cap.isOpened())  // if not success, exit program
    {
        cout << "ERROR: Cannot open the video file" << endl;
        return -1;
    }

 namedWindow("MyVideo",CV_WINDOW_AUTOSIZE); //create a window called "MyVideo"

   int dWidth = cap.get(CV_CAP_PROP_FRAME_WIDTH); //get the width of frames of the video
   int dHeight = cap.get(CV_CAP_PROP_FRAME_HEIGHT); //get the height of frames of the video

   cout << "Frame Size = " << dWidth << "x" << dHeight << endl;

   Size frameSize(dWidth, dHeight);

   //double cameraFPS = cap.get(CV_CAP_PROP_FPS);
/*
   Mat current;
   int count = 0;
   double fps = 0;
   while(true) {
       int64 start = cv::getTickCount();

       //Grab a frame
       cap >> current;
        count++;
       if(waitKey(3)>=0) {
           break;
       }

       fps += cv::getTickFrequency() / (cv::getTickCount() - start);
       std::cout << "FPS : " << fps/count << std::endl;
   }
*/

 VideoWriter oVideoWriter (argv[1], CV_FOURCC('M','P','4','2'), 24.0333, frameSize, true); //initialize the VideoWriter object

   if ( !oVideoWriter.isOpened() ) //if not initialize the VideoWriter successfully, exit the program
   {
        cout << "ERROR: Failed to write the video" << endl;
        return -1;
   }

   cout << "Starting to write the video\n";

    while (1)
    {

        Mat frame;

        bool bSuccess = cap.read(frame); // read a new frame from video

        if (!bSuccess) //if not success, break loop
       {
             cout << "ERROR: Cannot read a frame from video file" << endl;
             break;
        }

         oVideoWriter.write(frame); //writer the frame into the file

        imshow("MyVideo", frame); //show the frame in "MyVideo" window

        if (waitKey(1) == 27) //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
       {
            cout << "esc key is pressed by user" << endl;
            break;
       }
    }

    return 0;

}

