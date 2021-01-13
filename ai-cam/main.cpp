#include "yolo_cpu.hpp"

int main(int argc, char* argv[]) {
    if (argc != 7 && argc != 8) {
        printf("detector: Wrong number of arguments\n");
        printf("  <arg1>- .weights path\n");
        printf("  <arg2>- .cfg path\n");
        printf("  <arg3>- .names path\n");
        printf("  <arg4>- i/o image path\n");
        printf("  <arg5>- confidence threshold (default=0.4)\n");
        printf("  <arg6>- NMS threshold (default=0.5)\n");
        printf("  <arg7>- resize (default=416)\n");
        return -1;
    }
    const string modelPath = string(argv[1]);
    const string networkPath = string(argv[2]);
    const string classesPath = string(argv[3]);
    const string imagePath = string(argv[4]);
    int resize = 416;
    float confThreshold = 0.4, nmsThreshold = 0.5;
    if (argc == 6)  resize = atoi(argv[5]);
    if (argc == 7)  confThreshold = atof(argv[6]);
    if (argc == 8)  nmsThreshold = atof(argv[7]);
    
    Yolo_cpu yolo(modelPath, networkPath, classesPath, confThreshold, nmsThreshold, resize);
    if (yolo.detect(imagePath))   exit(SUCCESS_EVENT_TRUE);
    else                                exit(SUCCESS_EVENT_FALSE);
}