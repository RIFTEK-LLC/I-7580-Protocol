#include <iostream>
#include <chrono>
#include <ctime>
#include <CamsoChannel.h>

// Global variables.
int32_t serial_port = 14;
uint32_t bandwidth = 115200;

void ReadThreadFunc();

int main(void)
{
	// Dialog to enter serial port number.
    std::cout << "Enter first serial port number : ";
    std::cin >> serial_port;

    // Run threads.
    std::thread readThread(&ReadThreadFunc);

	// Wait threads.
    if (readThread.joinable())
        readThread.join();

	return 1;
}


void ReadThreadFunc()
{
    // Init channel.
    I7580::Channel* channel = new I7580::CamsoChannel();
    std::string initString = "\\\\.\\COM" + std::to_string(serial_port) + ";"
                                          + std::to_string(bandwidth);
    if (!channel->Init(initString))
    {
        std::cout << "Input channel not init" << std::endl;
        return;
    }

    // Allocate memory for data.
    uint8_t* inputData = new uint8_t[CAMSO_MAX_PACKET_SIZE];

    std::cout << std::endl;
    std::cout << "Start reading..." << std::endl;

    // Main loop.
    while (true)
    {
        // Wait data.
        uint32_t inputDataSize = 0;
        if (!channel->Get_Data(inputData, CAMSO_MAX_PACKET_SIZE, inputDataSize, 0, 1000))
        {
            std::cout << "No input data" << std::endl;
            continue;
        }

        std::cout << "port " << (int32_t)serial_port << "  " << inputDataSize << " bytes" << std::endl;

        // Compare input data value.

        bool calibration_start = (bool)inputData[0];
        if (calibration_start)
        {
            std::cout << "Calibration Start" << std::endl;
        }

        bool zero_positioning_start = (bool)inputData[1];
        if (zero_positioning_start)
        {
            std::cout << "Zero Positioning Start" << std::endl;
        }

        bool measuring_start = (bool)inputData[2];
        if (measuring_start)
        {
            std::cout << "Measuring Start" << std::endl;
        }

        bool abort_measure = (bool)inputData[3];
        if (abort_measure)
        {
            std::cout << "Abort Measure" << std::endl;
        }

        bool fault_reset = (bool)inputData[4];
        if (fault_reset)
        {
            std::cout << "Fault Reset" << std::endl;
        }

        bool save_measurements = (bool)inputData[5];
        if (save_measurements)
        {
            std::cout << "Save measurements" << std::endl;
        }

        bool new_data_transfer = (bool)inputData[6];
        if (new_data_transfer)
        {
            std::cout << "New Data Transfer "
                      << "(measuring_start_position = " << (*(uint32_t*)(&inputData[7]))
                      << ", calibration_diameter = " << (*(uint32_t*)(&inputData[11]))
                      << std::endl << std::endl;
        }

    }
}
