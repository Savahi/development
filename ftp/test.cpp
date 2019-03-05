#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>

int main(void) {
    CURLcode ret;
    CURL *curl = curl_easy_init();
    if (curl == NULL) {
        fprintf(stderr, "Failed creating CURL easy handle!\n");
        exit(EXIT_FAILURE);
    }

    /* let's get google.com because I love google */
    ret = curl_easy_setopt(curl, CURLOPT_URL, "http://www.google.com");
    if (ret != CURLE_OK) {
        fprintf(stderr, "Failed getting http://www.google.com: %s\n",
                curl_easy_strerror(ret));
        exit(EXIT_FAILURE);
    }

    ret = curl_easy_perform(curl);
    if (ret != 0) {
        fprintf(stderr, "Failed getting http://www.google.com: %s\n",
                curl_easy_strerror(ret));
        exit(EXIT_FAILURE);
    }

    return 0;
}