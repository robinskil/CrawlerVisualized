//
// Created by robin on 12-02-21.
//

#ifndef CRAWLERENGINE_CRAWLER_H
#define CRAWLERENGINE_CRAWLER_H

#include <stdbool.h>
#include "link_tree.h"
#include "crawler_settings.h"

void init_crawler();
link_node* start_crawling(crawler_settings *settings);

#endif //CRAWLERENGINE_CRAWLER_H
