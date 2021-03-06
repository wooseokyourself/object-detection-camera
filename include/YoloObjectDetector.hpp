#ifndef __YOLOOBJECTDETECTOR__
#define __YOLOOBJECTDETECTOR__

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
    void setModel (const std::string weightsPath, 
                   const std::string cfgPath, 
                   const std::string namesPath);
    Mat& getFrameRef ();
    Mat cloneFrame () const;
    std::string extractFrameBytes ();
    void extractImagefileBytes (std::string& outBytes, const std::string filePath);
    void resizeFrame (const int width);
    bool writeFrame (const std::string filePath) const;
    void capture (const int width);
    int detect (const int target, 
                const float confThreshold, 
                const float nmsThreshold, 
                const int resize);

private:
    void netPreProcess (const int resize, Size& padSize);
    int netPostProcess (const int target,
                        const float confThreshold, 
                        const float nmsThreshold,  
                        const Size& padSize, 
                        std::vector<Mat>& outs);

private:
    bool isSet;
    int target;

    Mat frame;
    Net net;

    std::vector<cv::String> outNames;
    std::vector<std::string> classes;
};

#endif