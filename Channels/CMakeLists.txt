cmake_minimum_required(VERSION 3.5)


##
## PROJECT
## name and version
##
project(Channels LANGUAGES CXX)



# add subdirectories of examples
if (CHANNEL_DEFAULT)
    add_subdirectory(DefaultChannel)
endif(CHANNEL_DEFAULT)

if (CHANNEL_CAMSO)
    add_subdirectory(CamsoChannel)
endif(CHANNEL_CAMSO)


