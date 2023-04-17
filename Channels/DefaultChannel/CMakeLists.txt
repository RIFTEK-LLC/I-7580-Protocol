cmake_minimum_required(VERSION 3.5)


##
## PROJECT
## name and version
##
project(DefaultChannel LANGUAGES CXX)


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
file (GLOB CPP_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
# concatenate the results (glob files) to variable
set  (SOURCES ${CPP_FILES} ${H_FILES})


##
## TARGET
## create target and add include path
##
add_library(${PROJECT_NAME} STATIC ${SOURCES})
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX  "")

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})


## add links to libraries
if(NOT WIN32)
    target_link_libraries(${PROJECT_NAME} pthread)
endif()
target_link_libraries(${PROJECT_NAME} Channel)
target_link_libraries(${PROJECT_NAME} SerialPort)
target_link_libraries(${PROJECT_NAME} DefaultParser)


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

# add_subdirectory(SerialChannelBenchmark)
