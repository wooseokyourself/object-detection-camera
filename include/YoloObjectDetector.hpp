#ifndef YOLOOBJECTDETECTOR
#define YOLOOBJECTDETECTOR

#include <cstring>
#include <iostream>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>

#include <opencv2/opencv.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>

using namespace cv;
using namespace dnn;

class YoloObjectDetector {
public:
    YoloObjectDetector ();
    void init (const std::string weightsPath, 
               const std::string cfgPath, 
               const std::String namesPath, 
               const int _target, 
               const float _confThreshold, 
               const float _nmsThreshold, 
               const int _width);
    Mat& getFrameRef ();
    Mat cloneFrame ();
    void capture ();
    int detect ();
    void getFrameBytes (std::string& outBytes) const;

private:
    void netPreProcess (Size& padSize);
    int netPostProcess (const Size& padSize, std::vector<Mat>& outs);

private:
    bool isSet;
    int target;

    Mat frame;
    Net net;

    std::vector<cv::String> outNames;
    std::vector<std::string> classes;

    float confThreshold;
    float nmsThreshold;
    int width;
};

#endif