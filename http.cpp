#include "http.hpp"

bool
postImage (const string& time,
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
                 CURLFORM_COPYCONTENTS, time,
                 CURLFORM_END);

    curl_formadd(&formpost,
                 &lastptr,
                 CURLFORM_COPYNAME, "event",
                 CURLFORM_COPYCONTENTS, "1",
                 CURLFORM_END);

    curl_formadd(&formpost,
                 &lastptr,
                 CURLFORM_COPYNAME, "rssi",
                 CURLFORM_COPYCONTENTS, str(rssi),
                 CURLFORM_END);

    curl_formadd(&formpost,
                 &lastptr,
                 CURLFORM_COPYNAME, "battery",
                 CURLFORM_COPYCONTENTS, str(battery),
                 CURLFORM_END);

    /* Fill in the filename field */
    curl_formadd(&formpost,
                &lastptr,
                CURLFORM_COPYNAME, "filename",
                CURLFORM_COPYCONTENTS, time + ".jpg",
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
    curl_easy_setopt(curl, CURLOPT_URL, "http://example.com/examplepost.cgi");
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

bool
postWithoutImage (const string& time,
                  const int& rssi,
                  const int& battery) {
    return false;
}