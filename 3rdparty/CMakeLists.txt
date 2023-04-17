cmake_minimum_required(VERSION 3.5)


##
## PROJECT
## name and version
##
project(3rdparty LANGUAGES CXX C)



# add subdirectories of examples
add_subdirectory(Channel)
add_subdirectory(Serialport)
add_subdirectory(endian_conv)

