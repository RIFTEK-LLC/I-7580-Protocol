cmake_minimum_required(VERSION 3.5)


##
## PROJECT
## name and version
##
project(CamsoExamples LANGUAGES CXX C)



# add subdirectories of examples
add_subdirectory(CamsoReceiver)
add_subdirectory(CamsoSender)
add_subdirectory(CamsoWorker)

