cmake_minimum_required(VERSION 3.5)


##
## PROJECT
## name and version
##
project(Channel)


##
## CONFIGURATION
##
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# Enabling export of all symbols to create a dynamic library
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)


# creating output directory architecture in accordance with GNU guidelines
set(BINARY_DIR "${CMAKE_BINARY_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${BINARY_DIR}/lib")


# create glob files for *.h, *.cpp
file (GLOB H_FILES   ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
# concatenate the results (glob files) to variable
set  (SOURCES ${H_FILES})


##
## TARGET
## create target and add include path
##
add_library(${PROJECT_NAME} INTERFACE)
target_sources(${PROJECT_NAME} INTERFACE ${SOURCES})

target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})


##
## INSTALL
## install header files, generate .lib or .dll file.
##
install(
    TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}
    LIBRARY DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/build/bin/
    RUNTIME DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/build/bin/
    CONFIGURATIONS DEBUG|RELEASE
    )
install(FILES ${H_FILES} DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/build/include)




