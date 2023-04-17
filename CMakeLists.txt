cmake_minimum_required(VERSION 3.5)


##
## PROJECT
## name and version
##
project(I7580 LANGUAGES CXX C)



# add subdirectories of examples
add_subdirectory(3rdparty)

SET(CHANNEL_DEFAULT  OFF CACHE BOOL "Add default channel" FORCE)
SET(CHANNEL_CAMSO    ON CACHE BOOL "Add Camso channel" FORCE)

add_subdirectory(Parsers)
add_subdirectory(Channels)

add_subdirectory(Examples)

