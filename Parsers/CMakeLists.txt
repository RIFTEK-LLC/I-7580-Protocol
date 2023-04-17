cmake_minimum_required(VERSION 3.5)


##
## PROJECT
## name and version
##
project(Parsers LANGUAGES CXX)



# add subdirectories of examples
if (CHANNEL_DEFAULT)
    add_subdirectory(DefaultParser)
endif(CHANNEL_DEFAULT)

if (CHANNEL_CAMSO)
    add_subdirectory(CamsoParser)
endif(CHANNEL_CAMSO)
