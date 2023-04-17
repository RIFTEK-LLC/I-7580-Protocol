cmake_minimum_required(VERSION 3.5)


##
## PROJECT
## name and version
##
project(Parsers LANGUAGES CXX C)



# add subdirectories of examples
if (CHANNEL_DEFAULT)
    add_subdirectory(DefaultExamples)
endif(CHANNEL_DEFAULT)

if (CHANNEL_CAMSO)
    add_subdirectory(CamsoExamples)
endif(CHANNEL_CAMSO)
