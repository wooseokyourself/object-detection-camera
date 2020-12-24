#include "yolo_cpu.hpp"

int main(int argc, char* argv[]) {
    if (argc != 5 && argc != 6) {
        printf("detector: Wrong number of arguments\n");
        printf("  <arg1>- .weights path\n");
        printf("  <arg2>- .cfg path\n");
        printf("  <arg3>- .names path\n");
        printf("  <arg4>- output image path\n");
        printf("  <arg5>- resize (default=416)\n");
        return -1;
    }
    const string modelPath = string(argv[1]);
    const string networkPath = string(argv[2]);
    const string classesPath = string(argv[3]);
    const string outputImagePath = string(argv[4]);
    int resize;
    if (argc == 6)
        resize = atoi(argv[5]);
    else
        resize = 416;
    
    Yolo_cpu yolo(modelPath, networkPath, classesPath);
    if (yolo.detect(outputImagePath, resize))
        exit(SUCCESS_EVENT_TRUE);
    else
        exit(SUCCESS_EVENT_FALSE);
}