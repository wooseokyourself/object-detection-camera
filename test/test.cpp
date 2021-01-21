#include <iostream>
#include <cstdio>
#include <cstring>
#include <vector>
#include <curl/curl.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include "date.h"

using namespace std;
using namespace cv;

template <class Precision> string
getISOCurrentTimestamp() {
    auto now = chrono::system_clock::now();
    return date::format("%FT%TZ", date::floor<Precision>(now));
}

bool
postImage (const string& url, 
           const string& time,
           const int& rssi,
           const int& battery, 
           const uchar* imgBuffPtr, 
           const long& imgBuffSize) {
    CURL* curl;
    CURLcode res;
    struct curl_httppost* formpost = NULL;
    struct curl_httppost* lastptr = NULL;
    struct curl_slist* headerlist = NULL;
    static const char buf[] = "Expect:";

    curl_global_init(CURL_GLOBAL_ALL);

    curl_formadd(&formpost,
                 &lastptr,
                 CURLFORM_COPYNAME, "time",
                 CURLFORM_COPYCONTENTS, time.c_str(),
                 CURLFORM_END);

    curl_formadd(&formpost,
                 &lastptr,
                 CURLFORM_COPYNAME, "event",
                 CURLFORM_COPYCONTENTS, "1",
                 CURLFORM_END);

    curl_formadd(&formpost,
                 &lastptr,
                 CURLFORM_COPYNAME, "rssi",
                 CURLFORM_COPYCONTENTS, to_string(rssi).c_str(),
                 CURLFORM_END);

    curl_formadd(&formpost,
                 &lastptr,
                 CURLFORM_COPYNAME, "battery",
                 CURLFORM_COPYCONTENTS, to_string(battery).c_str(),
                 CURLFORM_END);

    curl_formadd(&formpost,
                 &lastptr,
                 CURLFORM_COPYNAME, "filename",
                 CURLFORM_COPYCONTENTS, "wooseok-example.jpg",
                 CURLFORM_END);

    // from file
    curl_formadd(&formpost,
                 &lastptr,
                 CURLFORM_COPYNAME, "files",
                 CURLFORM_FILE, "wooseok-example.jpg",
                 CURLFORM_END);

/*  // from buffer = 이거 구현되면 결과사진 파이에 저장할 필요 없음
    curl_formadd(&formpost,
                &lastptr,
                CURLFORM_COPYNAME, "files",
                CURLFORM_BUFFERPTR, imgBuffPtr, 
                CURLFORM_BUFFERLENGTH, imgBuffSize,
                CURLFORM_END);
*/ 
    curl_formadd(&formpost,
                 &lastptr,
                 CURLFORM_COPYNAME, "submit",
                 CURLFORM_COPYCONTENTS, "send",
                 CURLFORM_END);

    curl = curl_easy_init();

    headerlist = curl_slist_append(headerlist, buf);
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);

        res = curl_easy_perform(curl);
        if(res != CURLE_OK) {
            // fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            return false;
        }
        curl_easy_cleanup(curl);
        curl_formfree(formpost);
        curl_slist_free_all (headerlist);
    }
    return true; // 이거 고치기
}

int main (void) {
    const string TIMESTAMP = getISOCurrentTimestamp<chrono::milliseconds>();
    const string URL = "http://ino-on.umilevx.com/api/devices/events/ino-on-0000";
    Mat frame = imread("wooseok-example.jpg");
    vector<uchar> imgBuffer;
    cv::imencode(".jpg", frame, imgBuffer);
    uchar* imgBuffPtr = &imgBuffer[0];
    long imgBuffSize = imgBuffer.size();
    postImage(URL, TIMESTAMP, 31, 99, imgBuffPtr, imgBuffSize);
    return 0;
}