#include "../include/YoloObjectDetector.hpp"

YoloObjectDetector::YoloObjectDetector () : isSet(false) {

}

void YoloObjectDetector::init (const std::string weightsPath, 
                               const std::string cfgPath, 
                               const std::String namesPath, 
                               const int _target, 
                               const float _confThreshold, 
                               const float _nmsThreshold, 
                               const int _width) {
    this->target = _target;
    try {
        this->net = readNet(weightsPath, cfgPath);
        this->net.setPreferableBackend(DNN_BACKEND_OPENCV);
        this->net.setPreferableTarget(DNN_TARGET_CPU);
        this->outNames = this->net.getUnconnectedOutLayersNames();

        ifstream ifs(names.c_str());
        std::string line;
        while (std::getline(ifs, line))
            classes.push_back(line);
    }
    catch (Exception& e) {
        std::cerr << "YoloObjectDetector: " << e.what() << std::endl;
    }
    this->isSet = true;
}

Mat& YoloObjectDetector::getFrameRef () {
    return this->frame;
}

Mat YoloObjectDetector::cloneFrame () {
    return this->frame.clone();
}

void YoloObjectDetector::capture () {
    try {
        VideoCapture cap;
        cap.open(0);
        cap.set(CAP_PROP_FRAME_WIDTH, this->width);
        cap.set(CAP_PROP_FRAME_HEIGHT, int((float(this->width) / 4) * 3));
        cap >> this->frame;
        cap.release();
    }
    catch (Exception& e) {
        std::cerr << "YoloObjectDetector: " << e.what() << std::endl;
    }
}

int YoloObjectDetector::detect () {
    if (!this->isSet) {
        std::cerr << "YoloObjectDetector: " << "need to initialize dnn model" << std::endl;
        return -1;
    }
    std::vector<Mat> outs;
    Size padSize;
    this->netPreProcess(padSize);
    net.forward(outs, this->outNames);
    return netPostProcess(padSize, outs);
}

void getFrameBytes (std::string& outBytes) const {
    int len = (this->frame.total() * this->frame.elemSize()) * sizeof(unsigned char);
    outBytes.resize(len);
    std::memcpy(outBytes, reinterpret_cast<char const*>(this->frame.data), len);
}

void YoloObjectDetector::netPreProcess (Size& padSize) {
    // Add padding to image
    if (this->frame.rows != this->frame.cols) {
        int length = this->frame.cols > this->frame.rows ? this->frame.cols : this->frame.rows;
        if (this->frame.cols < length) {
            padSize.width = length - this->frame.cols;
            padSize.height = length;
            Mat pad(padSize, this->frame.type(), Scalar(255, 255, 255));
            hconcat(pad, this->frame, this->frame);
        }
        else {
            padSize.width = length;
            padSize.height = length - this->frame.rows;
            Mat pad(padSize, this->frame.type(), Scalar(255, 255, 255));
            vconcat(pad, this->frame, this->frame);
        }
    }

    // Prepare for inference
    static Mat blob = blobFromImage(this->frame, 
                                    1, // scalarfactor: double
                                    Size(this->width, this->width),
                                    Scalar(), 
                                    true, // swapRB: bool
                                    false, 
                                    CV_8U);

    net.setInput(blob,
                 "", 
                 1/255.0, // scale: double
                 Scalar()); // mean: Scalar
}

int YoloObjectDetector::netPostProcess (const Size& padSize, std::vector<Mat>& outs) {
    int targetCount = 0;
    static std::vector<int> outLayers = net.getUnconnectedOutLayers();
    static std::string outLayerType = net.getLayer(outLayers[0])->type;

    std::vector<int> classIds;
    std::vector<float> confidences;
    std::vector<Rect> boxes;
    if (outLayerType == "Region") {
        for (size_t i = 0 ; i < outs.size() ; i ++) {
            float* data = (float*)outs[i].data;
            for (int j = 0 ; j < outs[i].rows ; j++, data += outs[i].cols) {
                Mat scores = outs[i].row(j).colRange(5, outs[i].cols);
                Point classIdPoint;
                double confidence;
                minMaxLoc(scores, 0, &confidence, 0, &classIdPoint);
                if (confidence > this->confThreshold) {
                    int width = (int)(data[2] * this->frame.cols);
                    int height = (int)(data[3] * this->frame.rows);
                    // Take a limit on size of the detecting boxes.
                    if (width >= this->frame.cols || height >= this->frame.rows)
                        continue;
                    int centerX = (int)(data[0] * this->frame.cols);
                    int centerY = (int)(data[1] * this->frame.rows);
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
        std::cerr << "YoloObjectDetector: " << "unknown output layer type: " << outLayerType << std::endl;

    // Draw rectangles
    std::vector<int> indices;
    NMSBoxes(boxes, confidences, this->confThreshold, this->nmsThreshold, indices);
    for (size_t i = 0 ; i < indices.size() ; i ++) {   
        int idx = indices[i];
        if (classIds[idx] == target) { // Draw rectangle if class is target
	    std::cout << "target: " << confidences[idx] << std::endl;
            targetCount++;
            Rect box = boxes[idx];
            int left = box.x;
            int top = box.y;
            int right = box.x + box.width;
            int bottom = box.y + box.height;
            int classId = classIds[idx];

            rectangle(this->frame, Point(left, top), Point(right, bottom), Scalar(0, 255, 0));
            string label = format("%.2f", confidences[idx]);
            if (!classes.empty())
                label = classes[classId] + ": " + label;
            int baseLine;
            Size labelSize = getTextSize(label, FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);
            top = max(top, labelSize.height);
            rectangle(frame, Point(left, top - labelSize.height), Point(left + labelSize.width, top + baseLine), Scalar::all(255), FILLED);
            putText(frame, label, Point(left, top), FONT_HERSHEY_SIMPLEX, 0.5, Scalar());
        }
    }

    // Get elapsed time
    std::vector<double> layersTimes;
    double freq = getTickFrequency() / 1000;
    double t = net.getPerfProfile(layersTimes) / freq;
    
    // Remove padding from image
    this->frame = this->frame(Range(padSize.height, this->frame.rows), Range(0, this->frame.cols));

    // Put the text for inference time
    std::string labelInferTime = format("Inference time: %.2f ms", t);
    putText(this->frame, labelInferTime, Point(0, 35), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 255), 2);
    // printf("Inference time: %.2f ms\n", t);
    return targetCount;
}