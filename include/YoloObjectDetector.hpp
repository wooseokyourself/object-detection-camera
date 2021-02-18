#ifndef YOLOOBJECTDETECTOR
#define YOLOOBJECTDETECTOR

#include <cassert>
#include <iostream>
#include <fstream>
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
    YoloObjectDetector (const std::string weightsPath, 
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

private:
    void netPreProcess (Size& padSize);
    int netPostProcess (const Size& padSize, std::vector<Mat>& outs);

private:
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