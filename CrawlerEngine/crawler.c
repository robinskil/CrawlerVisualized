//
// Created by robin on 12-02-21.
//
#include <curl/curl.h>
#include <stdlib.h>
#include <memory.h>
#include <libxml/HTMLparser.h>
#include <libxml/xpath.h>
#include <libxml/uri.h>
#include "crawler.h"

void crawl_node(link_node* node_ptr, crawler_settings* settings);

//We create a resizable buffer to pour the html in.
typedef struct {
    char* buffer;
    size_t size;
} resizable_buffer;

size_t grow_buffer(void *contents, size_t sz, size_t nmemb, void *buffer){
    size_t real_size = sz * nmemb;
    resizable_buffer *current_buffer = (resizable_buffer*) buffer;
    char *ptr = realloc(current_buffer->buffer, current_buffer->size + real_size);
    if(!ptr) {
        /* out of memory */
        printf("not enough memory (realloc returned NULL)\n");
        return 0;
    }
    current_buffer->buffer = ptr;
    memcpy(&(current_buffer->buffer[current_buffer->size]), contents, real_size);
    current_buffer->size += real_size;
    return real_size;
}

CURL* *create_webpage_handle(char *url, resizable_buffer *bufPtr) {
    //init curl
    CURL *handle;
    handle = curl_easy_init();

    //setup request parameters
    curl_easy_setopt(handle,CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2TLS);
    curl_easy_setopt(handle,CURLOPT_URL,url);

    //Setup resizable buffer to write the incoming data to.
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, grow_buffer);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, bufPtr);
    curl_easy_setopt(handle, CURLOPT_PRIVATE, bufPtr);

    //Other parameter settings for curl
    curl_easy_setopt(handle, CURLOPT_ACCEPT_ENCODING, "");
    curl_easy_setopt(handle, CURLOPT_TIMEOUT, 5L);
    curl_easy_setopt(handle, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(handle, CURLOPT_MAXREDIRS, 10L);
    curl_easy_setopt(handle, CURLOPT_CONNECTTIMEOUT, 2L);
    curl_easy_setopt(handle, CURLOPT_COOKIEFILE, "");
    curl_easy_setopt(handle, CURLOPT_FILETIME, 1L);
    curl_easy_setopt(handle, CURLOPT_USERAGENT, "Random Crawler");
    curl_easy_setopt(handle, CURLOPT_HTTPAUTH, CURLAUTH_ANY);
    curl_easy_setopt(handle, CURLOPT_UNRESTRICTED_AUTH, 1L);
    curl_easy_setopt(handle, CURLOPT_PROXYAUTH, CURLAUTH_ANY);
    curl_easy_setopt(handle, CURLOPT_EXPECT_100_TIMEOUT_MS, 0L);

    //Return the curl request handle
    return handle;
}

void find_hrefs_in_html_buffer(resizable_buffer* htmlBuffer, link_node* node_ptr, crawler_settings* settings_ptr){
    int parseOpts = HTML_PARSE_NOBLANKS | HTML_PARSE_NOERROR | HTML_PARSE_NOWARNING | HTML_PARSE_NONET;
    htmlDocPtr htmlDoc = htmlReadMemory(htmlBuffer->buffer, htmlBuffer->size, node_ptr->url, NULL, parseOpts);
    xmlChar* xpath = (xmlChar*) "//a/@href";
    xmlXPathContextPtr context = xmlXPathNewContext(htmlDoc);
    xmlXPathObjectPtr result = xmlXPathEvalExpression(xpath, context);
    xmlNodeSetPtr nodes = result->nodesetval;
    if(nodes != NULL){
        for(int i=0; i< nodes->nodeNr; i++){
            bool newly_visited = true;
            const xmlNode *node = nodes->nodeTab[i]->children;
            xmlChar* href = xmlNodeListGetString(htmlDoc, node, 1);
            if (node_ptr->same_site_depth_left > 0) {
                xmlChar *orig = href;
                href = xmlBuildURI(href, (xmlChar *) node_ptr->url);
                xmlFree(orig);
                newly_visited = false;
            }
            if (href != NULL) {
                char *link = (char *) href;
                if (link) {
                    if ((strncmp(link, "http://", 7) == 0 || strncmp(link, "https://", 8) == 0) &&
                        strcmp(link, node_ptr->url) != 0) {
                        link_node *child_node = create_child_node(link, newly_visited, node_ptr, settings_ptr);
                        add_child_node(node_ptr, child_node);
                        printf("%s\n", child_node->url);
                    }
                    xmlFree(link);
                }
            }
        }
    }
}

void crawl_page(link_node* node, crawler_settings* settings){
    //Create resBuffer in which we can store the html content
    if(node->web_search_depth_left > 0){
        resizable_buffer *resBuffer = malloc(sizeof(resizable_buffer));
        resBuffer->size = 0;
        resBuffer->buffer = malloc(1);

        CURLcode responseCode;
        CURL* pageHandle = create_webpage_handle(node->url, resBuffer);
        responseCode = curl_easy_perform(pageHandle);
        //If we get a 200 OK code we search hrefs in the html document
        if(responseCode == CURLE_OK){
            find_hrefs_in_html_buffer(resBuffer, node, settings);
        }
    }
    set_current_node_visited(node);
}

void init_crawler(){
    curl_global_init(CURL_GLOBAL_ALL);
}

void cleanup_crawler(){
    curl_global_cleanup();
}

link_node* start_crawling(crawler_settings *settings){
    link_node* start_node = create_initial_node(settings);
    crawl_node(start_node,settings);
    cleanup_crawler();
    return start_node;
}

void crawl_node(link_node* node_ptr, crawler_settings* settings){
    crawl_page(node_ptr, settings);
    for (int i = 0; i < node_ptr->children_size; ++i) {
        crawl_node(node_ptr->children[i],settings);
    }
}
