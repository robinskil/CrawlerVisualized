//
// Created by robin on 16-02-21.
//

#ifndef CRAWLERENGINE_LINK_TREE_H
#define CRAWLERENGINE_LINK_TREE_H

#include <stddef.h>
#include <stdbool.h>
#include "crawler_settings.h"

typedef struct link_node link_node;
struct link_node {
    link_node* parent;
    link_node** children;
    char* url;
    int children_size;
    int children_allocated_size;
    int same_site_depth_left;
    int web_search_depth_left;
    bool crawled_current_url;
    bool newly_visited_website;
};

link_node* create_initial_node(crawler_settings* settings);
void set_current_node_visited(link_node *node_ptr);
link_node* create_child_node(char* current_url, bool newly_visited_website, link_node* parent_ptr, crawler_settings* crawler_settings);
void clear_node_tree(link_node* head_ptr);
void add_child_node(link_node* parent_node, link_node* child_node);

#endif //CRAWLERENGINE_LINK_TREE_H
