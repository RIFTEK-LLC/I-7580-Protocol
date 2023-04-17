cmake_minimum_required(VERSION 3.5)


##
## PROJECT
## name and version
##
project(CamsoWorker_Lib LANGUAGES CXX C)


##
## CONFIGURATION
##
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# Enabling export of all symbols to create a dynamic library
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Core Gui Widgets LinguistTools Xml OpenGL SerialPort REQUIRED)

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

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
qt5_use_modules(${PROJECT_NAME}
    Core
    Gui
    Qml
    Quick
    QuickControls2
    Concurrent
    Charts
    Widgets
    PrintSupport
    LinguistTools
    )

## add links to libraries
if(NOT WIN32)
    target_link_libraries(${PROJECT_NAME} pthread)
endif()
if(MINGW)
    target_link_libraries(${PROJECT_NAME} ws2_32 winmm)
    target_link_libraries(${PROJECT_NAME} iphlpapi)
endif(MINGW)

target_link_libraries(${PROJECT_NAME} Channel)
target_link_libraries(${PROJECT_NAME} SerialPort)
target_link_libraries(${PROJECT_NAME} CamsoParser)
target_link_libraries(${PROJECT_NAME} CamsoChannel)

target_link_libraries(${PROJECT_NAME} endian_conv)
