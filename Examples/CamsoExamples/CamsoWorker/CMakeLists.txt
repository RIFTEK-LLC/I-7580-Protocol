cmake_minimum_required(VERSION 3.5)


##
## PROJECT
## name and version
##
project(CamsoWorker LANGUAGES CXX C)

add_subdirectory(CamsoWorker_Lib)

add_subdirectory(TestCamsoSystem)
add_subdirectory(TestCamsoPLC)
