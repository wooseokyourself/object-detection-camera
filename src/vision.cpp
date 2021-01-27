#include "../include/vision.hpp"

void
vision::capture (Mat& frame, const int& resize) {
    try {
        VideoCapture cap;
        cap.open(0);
        cap.set(CAP_PROP_FRAME_WIDTH, resize);
        cap.set(CAP_PROP_FRAME_HEIGHT, int((float(resize) / 4) * 3));
        cap >> frame;
        cap.release();
    }
    catch (Exception& e) {
        vision::__Assert(false, FAILURE_READING);
    }
}

bool
vision::detect (Mat& frame,
        const string& weight, 
        const string& cfg, 
        const string& names, 
        const float& confThreshold, 
        const float& nmsThreshold, 
        const int& resize) {
    vision::__Assert(!frame.empty(), FAILURE_READING);
    vector<Mat> outs;
    Net net;
    vector<cv::String> outNames;
    vector<string> classes;
    Size padSize;
    try {
        net = readNet(weight, cfg);
        net.setPreferableBackend(DNN_BACKEND_OPENCV);
        net.setPreferableTarget(DNN_TARGET_CPU);
        outNames = net.getUnconnectedOutLayersNames();

        ifstream ifs(names.c_str());
        string line;
        while (std::getline(ifs, line))
            classes.push_back(line);
    }
    catch (Exception& e) {
        vision::__Assert(false, FAILURE_MODEL_LOADING);
    }

    bool isDetected = false;
    vision::__netPreProcess(frame, padSize, resize, net);
    net.forward(outs, outNames);
    if (vision::__netPostProcess(frame, padSize, net, outs, confThreshold, nmsThreshold, classes) != 0)
        isDetected = true;
    return isDetected;
}

void
vision::__netPreProcess (Mat& frame, 
                 Size& padSize, 
                 const int& resize, 
                 Net& net) {
    //MARK: Add padding to image
    if (frame.rows != frame.cols) {
        int length = frame.cols > frame.rows ? frame.cols : frame.rows;
        if (frame.cols < length) {
            padSize.width = length - frame.cols;
            padSize.height = length;
            Mat pad(padSize, frame.type(), Scalar(255, 255, 255));
            hconcat(pad, frame, frame);
        }
        else {
            padSize.width = length;
            padSize.height = length - frame.rows;
            Mat pad(padSize, frame.type(), Scalar(255, 255, 255));
            vconcat(pad, frame, frame);
        }
    }

    //MARK: Prepare for inference
    static Mat blob = blobFromImage(frame, 
                                    1, // scalarfactor: double
                                    Size(resize, resize),
                                    Scalar(), 
                                    true, // swapRB: bool
                                    false, 
                                    CV_8U);

    net.setInput(blob,
                 "", 
                 1/255.0, // scale: double
                 Scalar()); // mean: Scalar
}

int
vision::__netPostProcess (Mat& frame, 
                  const Size& padSize, 
                  Net& net, 
                  const vector<Mat>& outs, 
                  const float& confThreshold, 
                  const float& nmsThreshold, 
                  const vector<string>& classes) {
    int excavatorCount = 0;
    static vector<int> outLayers = net.getUnconnectedOutLayers();
    static string outLayerType = net.getLayer(outLayers[0])->type;

    vector<int> classIds;
    vector<float> confidences;
    vector<Rect> boxes;
    if (outLayerType == "Region") {
        for (size_t i = 0; i < outs.size(); ++i) {
            float* data = (float*)outs[i].data;
            for (int j = 0; j < outs[i].rows; ++j, data += outs[i].cols) {
                Mat scores = outs[i].row(j).colRange(5, outs[i].cols);
                Point classIdPoint;
                double confidence;
                minMaxLoc(scores, 0, &confidence, 0, &classIdPoint);
                if (confidence > confThreshold) {
                    int width = (int)(data[2] * frame.cols);
                    int height = (int)(data[3] * frame.rows);
                    // Take a limit on size of the detecting boxes.
                    if (width * height >= (frame.cols * frame.rows) / 2
                        || width >= frame.cols * 4 / 5
                        || height >= frame.cols * 4 / 5)
                        continue;
                    int centerX = (int)(data[0] * frame.cols);
                    int centerY = (int)(data[1] * frame.rows);
                    int left = centerX - width / 2;
                    int top = centerY - height / 2;
                    classIds.push_back(classIdPoint.x);
                    confidences.push_back((float)confidence);
                    boxes.push_back(Rect(left, top, width, height));
                }
            }
        }
    }
    else
        CV_Error(Error::StsNotImplemented, "Unknown output layer type: " + outLayerType);

    //MARK: Draw rectangle
    vector<int> indices;
    NMSBoxes(boxes, confidences, confThreshold, nmsThreshold, indices);
    for (size_t i = 0; i < indices.size(); ++i) {   
        int idx = indices[i];
        if (classIds[idx] == 0) { // Draw rectangle if class is for excavator.
            excavatorCount ++;
            Rect box = boxes[idx];
            int left = box.x;
            int top = box.y;
            int right = box.x + box.width;
            int bottom = box.y + box.height;
            int classId = classIds[idx];

            rectangle(frame, Point(left, top), Point(right, bottom), Scalar(0, 255, 0));
            string label = format("%.2f", confidences[idx]);
            if (!classes.empty()) {
                vision::__Assert(classId < (int)classes.size(), FAILURE_MODEL_LOADING);
                label = classes[classId] + ": " + label;
            }
            int baseLine;
            Size labelSize = getTextSize(label, FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);
            top = max(top, labelSize.height);
            rectangle(frame, Point(left, top - labelSize.height), Point(left + labelSize.width, top + baseLine), Scalar::all(255), FILLED);
            putText(frame, label, Point(left, top), FONT_HERSHEY_SIMPLEX, 0.5, Scalar());
        }
    }

    //MARK: Get elapsed time
    vector<double> layersTimes;
    double freq = getTickFrequency() / 1000;
    double t = net.getPerfProfile(layersTimes) / freq;
    
    //MARK: Remove padding from image
    frame = frame(Range(padSize.height, frame.rows), Range(0, frame.cols));

    //MARK: Put the text for inference time
    string labelInferTime = format("Inference time: %.2f ms", t);
    putText(frame, labelInferTime, Point(0, 35), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 255), 2);
    // printf("Inference time: %.2f ms\n", t);
    return excavatorCount;
}

void 
vision::__Assert (bool condition, int status) {
    if (!condition) {
        printf("ino-on_AICamera %d", status);
        exit(status);
    }
}