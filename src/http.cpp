#include "../include/http.hpp"

/// @brief 관리자모드에서 사용되는 POST
bool
http::post (const string& url, 
            const strimg& time, 
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
                 CURLFORM_COPYNAME, "filename",
                 CURLFORM_COPYCONTENTS, filename.c_str(),
                 CURLFORM_END);

    // from file
    curl_formadd(&formpost,
                 &lastptr,
                 CURLFORM_COPYNAME, "files",
                 CURLFORM_FILE, filepath.c_str(),
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
        while (res != CURLE_OK && res != CURLE_OPERATION_TIMEDOUT) {
            res = curl_easy_perform(curl);
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            delay(2500);
        }
        curl_easy_cleanup(curl);
        curl_formfree(formpost);
        curl_slist_free_all (headerlist);
        if (res != CURLE_OPERATION_TIMEDOUT)
            return true;
        else
            return false;
    }
    else
        return false;
}

/// @brief 일반모드에서 이벤트가 감지되었을때 사용되는 POST
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
        while (res != CURLE_OK && res != CURLE_OPERATION_TIMEDOUT) {
            res = curl_easy_perform(curl);
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            delay(2500);
        }
        curl_easy_cleanup(curl);
        curl_formfree(formpost);
        curl_slist_free_all (headerlist);
        if (res != CURLE_OPERATION_TIMEDOUT)
            return true;
        else
            return false;
    }
    else
        return false;
}

/// @brief 일반모드에서 이벤트가 감지되지 않았을때 사용되는 POST
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
        while (res != CURLE_OK && res != CURLE_OPERATION_TIMEDOUT) {
            res = curl_easy_perform(curl);
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            delay(2500);
        }
        curl_easy_cleanup(curl);
        curl_formfree(formpost);
        curl_slist_free_all (headerlist);
        if (res != CURLE_OPERATION_TIMEDOUT)
            return true;
        else
            return false;
    }
    else
        return false;
}