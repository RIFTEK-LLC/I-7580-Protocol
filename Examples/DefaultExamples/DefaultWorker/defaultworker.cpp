#include "profinetworker.h"

#include "windows.h"
#include <iostream>
#include <chrono>
#include <ctime>

#include <SerialChannel.h>

ProfinetWorker::ProfinetWorker(uint32_t bandwidth, int32_t serial_port, QObject *parent) : QObject(parent)
{
    m_Bandwidth = bandwidth;
    m_SerialPort = serial_port;
}

ProfinetWorker::~ProfinetWorker()
{

}

void ProfinetWorker::ReadThreadFunc()
{
    isRun = true;
    // Init channel.
    I7580::Channel* channel = new I7580::SerialChannel();
    std::string initString = "\\\\.\\COM" + std::to_string(m_SerialPort) + ";" + std::to_string(m_Bandwidth);
    if (!channel->Init(initString))
    {
        std::cout << "Input channel not init" << std::endl;
        return;
    }

    // Allocate memory for data.
    uint8_t* inputData = new uint8_t[1048576];

    std::cout << std::endl;
    std::cout << "Start reading..." << std::endl;

    // Main loop.
    while (isRun)
    {
        // Wait data.
        uint32_t inputDataSize = 0;
        if (!channel->Get_Data(inputData, 1048576, inputDataSize, 0, 1000))
        {
            std::cout << "No input data" << std::endl;
            continue;
        }

        std::cout << "port " << (int32_t)m_SerialPort << "  " << inputDataSize << " bytes" << std::endl;

        // Compare input data value.

        bool calibration_start = (bool)inputData[0];
        if (calibration_start)
        {
            emit CalibrationStart();
        }

        bool zero_positioning_start = (bool)inputData[1];
        if (zero_positioning_start)
        {
            emit ZeroPositioningStart();
        }

        bool measuring_start = (bool)inputData[2];
        if (measuring_start)
        {
            emit MeasuringStart();
        }

        bool abort_measure = (bool)inputData[3];
        if (abort_measure)
        {
            emit AbortMeasure();
        }

        bool fault_reset = (bool)inputData[4];
        if (fault_reset)
        {
            emit FaultReset();
        }

        bool new_data_transfer = (bool)inputData[5];
        if (new_data_transfer)
        {
            emit NewDataTransfer(*(uint32_t*)(&inputData[6]), *(uint32_t*)(&inputData[10]));
        }

    }

}

void ProfinetWorker::stop()
{
    isRun = false;
}
