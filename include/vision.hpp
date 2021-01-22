#ifndef VISION
#define VISION

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
    FAILURE_READING         = 2,
    FAILURE_WRITING         = 3,
    FAILURE_MODEL_LOADING   = 4,
    FAILURE_INFERENCE       = 5
};

namespace vision {

void
capture (Mat& frame, const int& resize);

bool
detect (Mat& frame,
        const string& weight, 
        const string& cfg, 
        const string& names, 
        const float& confThreshold, 
        const float& nmsThreshold, 
        const int& resize);

void
__netPreProcess (Mat& frame, 
                 Size& padSize, 
                 const int& resize, 
                 Net& net);
                    
int
__netPostProcess (Mat& frame, 
                  const Size& padSize, 
                  Net& net, 
                  const vector<Mat>& outs, 
                  const float& confThreshold, 
                  const float& nmsThreshold, 
                  const vector<string>& classes);

void
__Assert (bool condition, int status);

}
#endif