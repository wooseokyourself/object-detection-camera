#include "yolo_cpu.hpp"

void Assert (bool condition, int status) {
    if (!condition)
        exit(status);
}

Yolo_cpu::Yolo_cpu (const string _MODEL_PATH, const string _NETWORK_PATH, const string _CLASSES_PATH) {
    try {
        this->MODEL_PATH = _MODEL_PATH;
        this->NETWORK_PATH = _NETWORK_PATH;
        this->CLASSES_PATH = _CLASSES_PATH;

        this->confThreshold = 0.4;
        this->nmsThreshold = 0.5;

        this->net = readNet(MODEL_PATH, NETWORK_PATH);
        this->net.setPreferableBackend(DNN_BACKEND_OPENCV);
        this->net.setPreferableTarget(DNN_TARGET_CPU);
        this->outNames = net.getUnconnectedOutLayersNames();

        ifstream ifs(CLASSES_PATH.c_str());
        string line;
        while (std::getline(ifs, line))
            this->classes.push_back(line);
    }
    catch (Exception& e) {
        Assert(false, FAILURE_MODEL_LOADING);
    }
}

bool Yolo_cpu::detect (const string outputImagePath, const int resize) {
    bool isDetected = false;
    Mat frame;
    frame = getCam();
    vector<Mat> outs;

    preProcess(frame, resize);
    net.forward(outs, outNames);
    if (postProcess(frame, outs) != 0)
        isDetected = true;

    vector<double> layersTimes;
    double freq = getTickFrequency() / 1000;
    double t = net.getPerfProfile(layersTimes) / freq;
    
    string labelInferTime = format("Inference time: %.2f ms", t);
    putText(frame, labelInferTime, Point(0, 35), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 255), 2);

    imwrite(outputImagePath, frame);
    
    printf("Inference time: %.2f ms\n", t);
    return isDetected;
}

Mat Yolo_cpu::getCam () {
    Mat frame;
    try {
        VideoCapture cap;
        cap.open(0);
        cap >> frame;
        cap.release();
    }
    catch (Exception& e) {
        Assert(false, FAILURE_CAMERA);
    }
    return frame;
}

void Yolo_cpu::preProcess (Mat& frame, const int resize) {
    //MARK: Image padding
    if (frame.rows != frame.cols) {
        int length = frame.cols > frame.rows ? frame.cols : frame.rows;
        if (frame.cols < length) {
            Mat pad(length, length - frame.cols, frame.type(), Scalar(255, 255, 255));
            hconcat(pad, frame, frame);
        }
        else {
            Mat pad(length - frame.rows, length, frame.type(), Scalar(255, 255, 255));
            vconcat(pad, frame, frame);
        }
    }

    //MARK: Prepare for inference
    static Mat blob = blobFromImage(frame, 
                                    1, // scalarfactor: double
                                    Size(resize, resize), // resizeRes: Size
                                    Scalar(), 
                                    true, // swapRB: bool
                                    false, 
                                    CV_8U);

    net.setInput(blob,
                 "", 
                 1/255.0, // scale: double
                 Scalar()); // mean: Scalar
}

int Yolo_cpu::postProcess (Mat& frame, const vector<Mat> outs) {
    int excavatorCount = 0;
    static vector<int> outLayers = net.getUnconnectedOutLayers();
    static string outLayerType = net.getLayer(outLayers[0])->type;

    vector<int> classIds;
    vector<float> confidences;
    vector<Rect> boxes;
    if (outLayerType == "DetectionOutput") {
        // Network produces output blob with a shape 1x1xNx7 where N is a number of
        // detections and an every detection is a vector of values
        // [batchId, classId, confidence, left, top, right, bottom]
        for (size_t k = 0; k < outs.size(); k++) {
            float* data = (float*)outs[k].data;
            for (size_t i = 0; i < outs[k].total(); i += 7) {
                float confidence = data[i + 2];
                if (confidence > confThreshold) {
                    int left   = (int)data[i + 3];
                    int top    = (int)data[i + 4];
                    int right  = (int)data[i + 5];
                    int bottom = (int)data[i + 6];
                    int width  = right - left + 1;
                    int height = bottom - top + 1;
                    if (width <= 2 || height <= 2) {
                        left   = (int)(data[i + 3] * frame.cols);
                        top    = (int)(data[i + 4] * frame.rows);
                        right  = (int)(data[i + 5] * frame.cols);
                        bottom = (int)(data[i + 6] * frame.rows);
                        width  = right - left + 1;
                        height = bottom - top + 1;
                    }
                    classIds.push_back((int)(data[i + 1]) - 1);  // Skip 0th background class id.
                    boxes.push_back(Rect(left, top, width, height));
                    confidences.push_back(confidence);
                }
            }
        }
    }
    else if (outLayerType == "Region") {
        for (size_t i = 0; i < outs.size(); ++i) {
            // Network produces output blob with a shape NxC where N is a number of
            // detected objects and C is a number of classes + 4 where the first 4
            // numbers are [center_x, center_y, width, height]
            float* data = (float*)outs[i].data;
            for (int j = 0; j < outs[i].rows; ++j, data += outs[i].cols) {
                Mat scores = outs[i].row(j).colRange(5, outs[i].cols);
                Point classIdPoint;
                double confidence;
                minMaxLoc(scores, 0, &confidence, 0, &classIdPoint);
                if (confidence > confThreshold) {
                    int centerX = (int)(data[0] * frame.cols);
                    int centerY = (int)(data[1] * frame.rows);
                    int width = (int)(data[2] * frame.cols);
                    int height = (int)(data[3] * frame.rows);
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
                Assert(classId < (int)classes.size(), FAILURE_MODEL_LOADING);
                label = classes[classId] + ": " + label;
            }
            int baseLine;
            Size labelSize = getTextSize(label, FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);
            top = max(top, labelSize.height);
            rectangle(frame, Point(left, top - labelSize.height), Point(left + labelSize.width, top + baseLine), Scalar::all(255), FILLED);
            putText(frame, label, Point(left, top), FONT_HERSHEY_SIMPLEX, 0.5, Scalar());
        }
    }
    return excavatorCount;
}
