#include "../include/http.hpp"

bool
http::post (const string& url, 
            const string& time,
            const int& rssi,
            const int& battery, 
            const string& filename, 
            const string& filepath, 
            const long& timeoutsecs) {

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
                 CURLFORM_COPYCONTENTS, filename.c_str(),
                 CURLFORM_END);

    // from file
    curl_formadd(&formpost,
                 &lastptr,
                 CURLFORM_COPYNAME, "files",
                 CURLFORM_FILE, filepath.c_str(),
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

    CURL* curl = curl_easy_init();

    headerlist = curl_slist_append(headerlist, buf);
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeoutsecs);

        CURLcode res = curl_easy_perform(curl);
        while (res != CURLE_OK) {
            res = curl_easy_perform(curl);
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            delay(2500);
        }
        
        curl_easy_cleanup(curl);
        curl_formfree(formpost);
        curl_slist_free_all (headerlist);
    }
    return true;
}

bool
http::post (const string& url, 
            const string& time,
            const int& rssi,
            const int& battery, 
            const long& timeoutsecs) {

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
                 CURLFORM_COPYCONTENTS, "0",
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
                 CURLFORM_COPYNAME, "submit",
                 CURLFORM_COPYCONTENTS, "send",
                 CURLFORM_END);

    CURL* curl = curl_easy_init();

    headerlist = curl_slist_append(headerlist, buf);
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeoutsecs);

        CURLcode res = curl_easy_perform(curl);
        while (res != CURLE_OK) {
            res = curl_easy_perform(curl);
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            delay(2500);
        }
    
        curl_easy_cleanup(curl);
        curl_formfree(formpost);
        curl_slist_free_all (headerlist);
    }
    return true;
}