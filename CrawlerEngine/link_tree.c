//
// Created by robin on 16-02-21.
//

#include <string.h>
#include <stdlib.h>
#include "link_tree.h"

const int default_children_allocated_size = 8;

void grow_children_array(link_node* parent_node);

link_node* create_initial_node(crawler_settings* settings) {
    link_node* node_ptr = malloc(sizeof(link_node));
    node_ptr->url = (char*)malloc(sizeof(char*));
    node_ptr->web_search_depth_left = settings->web_search_depth;
    node_ptr->same_site_depth_left = settings->same_site_search_depth;
    node_ptr->children_size = 0;
    node_ptr->children = malloc(sizeof(link_node*)*default_children_allocated_size);
    node_ptr->children_allocated_size = default_children_allocated_size;
    node_ptr->newly_visited_website = true;
    //Copy the char ptr value url so we dont have a possible double free or memory leak.
    //Now the node has full ownership of the url and clears it by it self
    strcpy(node_ptr->url, settings->start_url);
    return node_ptr;
};

//Sets the url of children nodes and the size of the children in the node_ptr
void set_current_node_visited(link_node *node_ptr) {
    node_ptr->crawled_current_url = true;
}

link_node* create_child_node(char* current_url, bool newly_visited_website, link_node* parent_ptr, crawler_settings* crawler_settings){
    link_node* child_node_ptr = (link_node*)malloc(sizeof(link_node));
    child_node_ptr->parent = parent_ptr;
    child_node_ptr->crawled_current_url = false;
    child_node_ptr->newly_visited_website = newly_visited_website;
    child_node_ptr->url = (char*)malloc(strlen(current_url)+1);
    child_node_ptr->children = (link_node**) malloc(sizeof(link_node**));
    child_node_ptr->children_size = 0;
    child_node_ptr->children_allocated_size = 0;
    //Copy the char ptr value url so we dont have a possible double free or memory leak.
    //Now the node has full ownership of the url and clears it by it self
    strcpy(child_node_ptr->url, current_url);
    if(newly_visited_website){
        child_node_ptr->web_search_depth_left = parent_ptr->web_search_depth_left-1;
        child_node_ptr->same_site_depth_left = crawler_settings->same_site_search_depth;
    }
    else {
        child_node_ptr->web_search_depth_left = parent_ptr->web_search_depth_left;
        child_node_ptr->same_site_depth_left = parent_ptr->same_site_depth_left-1;
    }
    return child_node_ptr;
}

void add_child_node(link_node* parent_node, link_node* child_node){
    if(parent_node->children_allocated_size > parent_node->children_size) {
        parent_node->children[parent_node->children_size] = child_node;
        parent_node->children_size+=1;
    }
    else {
        grow_children_array(parent_node);
        add_child_node(parent_node,child_node);
    }
}

void grow_children_array(link_node* parent_node){
    link_node** new_ptr = (link_node**) realloc(parent_node->children,sizeof(link_node*) * (parent_node->children_allocated_size+8));
    //Assign the new ptr to the array
    parent_node->children = new_ptr;
    parent_node->children_allocated_size += 8;
}

void free_node(link_node* node){
    free(node->url);
    for (int i = 0; i < node->children_size; ++i) {
        free_node(node->children[i]);
    }
    free(node);
}

void clear_node_tree(link_node* head_ptr){
    free_node(head_ptr);
}