//
// Created by robin on 17-02-21.
//

#ifndef CRAWLERENGINE_CRAWLER_SETTINGS_H
#define CRAWLERENGINE_CRAWLER_SETTINGS_H

typedef struct {
    int same_site_search_depth;
    int web_search_depth;
    char* start_url;
} crawler_settings;


#endif //CRAWLERENGINE_CRAWLER_SETTINGS_H
