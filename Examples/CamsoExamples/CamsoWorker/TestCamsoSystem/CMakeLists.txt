cmake_minimum_required(VERSION 3.5)

project(TestCamsoSystem LANGUAGES CXX C)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enabling export of all symbols to create a dynamic library
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# creating output directory architecture in accordance with GNU guidelines
set(BINARY_DIR "${CMAKE_BINARY_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${BINARY_DIR}/lib")

find_package(Qt5 COMPONENTS Core Gui Widgets Xml Svg OpenGL SerialPort REQUIRED)

# create glob files for *.h, *.cpp
file(GLOB_RECURSE SOURCES LIST_DIRECTORIES false *.h *.cpp *.qrc *.ui)

# concatenate the results (glob files) to variable
set(SRC_FILES ${SOURCES})

add_executable(${PROJECT_NAME} WIN32 ${SRC_FILES} ${TS_FILES})

target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Widgets Qt5::Xml Qt5::OpenGL Qt5::SerialPort)

target_link_libraries(${PROJECT_NAME} Channel)
target_link_libraries(${PROJECT_NAME} SerialPort)
target_link_libraries(${PROJECT_NAME} CamsoChannel)
target_link_libraries(${PROJECT_NAME} CamsoWorker_Lib)

target_link_libraries(${PROJECT_NAME} endian_conv)
