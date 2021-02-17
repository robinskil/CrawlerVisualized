cmake_minimum_required(VERSION 3.17)
project(CrawlerEngine C)

find_package(LibXml2 REQUIRED)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ ${LIBXML2_INCLUDE_DIR})

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -fexceptions -g")

add_executable(CrawlerEngine main.c crawler.c crawler.h link_tree.c link_tree.h crawler_settings.h)
target_link_libraries(CrawlerEngine -lcurl ${LIBXML2_LIBRARIES})