#ifndef YOLO_CPU
#define YOLO_CPU

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include <opencv2/opencv.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>

using namespace std;
using namespace cv;
using namespace dnn;

enum STATUS {
    SUCCESS_EVENT_FALSE     = 0,
    SUCCESS_EVENT_TRUE      = 1,
    FAILURE_CAMERA          = 2,
    FAILURE_WRITING         = 3,
    FAILURE_MODEL_LOADING   = 4,
    FAILURE_INFERENCE       = 5
};

void Assert (bool condition, int status);

class Yolo_cpu {
private:
    string MODEL_PATH;
    string NETWORK_PATH;
    string CLASSES_PATH;
    float confThreshold;
    float nmsThreshold;
    int resize;
    Net net;
    vector<cv::String> outNames;
    vector<string> classes;
    Size padSize;
    
public:
    Yolo_cpu (const string _MODEL_PATH, const string _NETWORK_PATH, const string _CLASSES_PATH, const float _confThreshold, const float _nmsThreshold, const int _resize);
    bool detect (const string imagePath); /** returns which the excavator was detected or not */

private:
    void netPreProcess (Mat& frame);
    int netPostProcess (Mat& frame, const vector<Mat> outs); /** returns the number of detected excavators */
};

#endif
