#include <stdio.h>
#include "crawler.h"

int main() {
    init_crawler();
    crawler_settings settings = {
            1,
            2,
            "https://www.dell.com"
    };
    link_node* initial_node = start_crawling(&settings);
    clear_node_tree(initial_node);
    return 0;
}

