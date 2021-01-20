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
           const char* imgBuffPtr, 
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

    /* Fill in the filename field */
    curl_formadd(&formpost,
                &lastptr,
                CURLFORM_COPYNAME, "filename",
                CURLFORM_COPYCONTENTS, (time + ".jpg").c_str(),
                CURLFORM_END);

    /* Fill in the file upload field */
    curl_formadd(&formpost,
                &lastptr,
                CURLFORM_COPYNAME, "files",
                CURLFORM_BUFFERPTR, imgBuffPtr, 
                CURLFORM_BUFFERLENGTH, imgBuffSize,
                CURLFORM_END);

    curl = curl_easy_init();

    /* initalize custom header list (stating that Expect: 100-continue is not
     wanted */
    headerlist = curl_slist_append(headerlist, buf);
    if(curl) {
    /* what URL that receives this POST */
    curl_easy_setopt(curl, CURLOPT_URL, url);
    if ( (argc == 2) && (!strcmp(argv[1], "noexpectheader")) )
        /* only disable 100-continue header if explicitly requested */
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);
    curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);

    /* Perform the request, res will get the return code */
    res = curl_easy_perform(curl);
    /* Check for errors */
    if(res != CURLE_OK)
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));

    /* always cleanup */
    curl_easy_cleanup(curl);

    /* then cleanup the formpost chain */
    curl_formfree(formpost);
    /* free slist */
    curl_slist_free_all (headerlist);
    }
    return true; // 이거 고치기
}

int main (void) {
    const string TIMESTAMP = getISOCurrentTimestamp<chrono::milliseconds>();
    const string URL = "http://ino-on.umilevx.com/api/devices/events/ino-on-0000"
    Mat frame = imread("example.jpg");
    vector<uchar> imgBuffer;
    cv::imencode(".jpg", frame, imgBuffer);
    char* imgBuffPtr = &imgBuffer[0];
    long imgBuffSize = imgBuffer.size();
    postImage(URL, TIMESTAMP, 31, 99, imgBuffPtr, imgBuffSize);
    return 0;
}