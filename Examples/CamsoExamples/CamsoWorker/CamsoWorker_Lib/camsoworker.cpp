#include "camsoworker.h"

#include "windows.h"
#include <iostream>
#include <chrono>
#include <ctime>

#include <endian_conv.h>

CamsoWorker::CamsoWorker(int32_t serial_port, uint32_t baudrate, QObject *parent) : QObject(parent)
  , m_TimeoutMs(500)
{
    m_Baudrate = baudrate;
    m_SerialPort = serial_port;

    // Init channel.
    channel = new I7580::CamsoChannel();
    std::string initString = "\\\\.\\COM" + std::to_string(m_SerialPort) + ";" + std::to_string(m_Baudrate);
    if (!channel->Init(initString))
    {
        std::cout << "Input channel not init" << std::endl;
        return;
    }

    isInit = true;
}

CamsoWorker::~CamsoWorker()
{
    delete channel;
}

bool CamsoWorker::SendData(system_packet packet)
{
    uint8_t outputData[75] = {
        packet.state.calibration_running,
        packet.state.calibration_completed,
        packet.state.calibration_error,

        packet.state.zero_positioning_running,
        packet.state.zero_positioning_completed,
        packet.state.zero_positioning_error,

        packet.state.measuring_running,
        packet.state.measuring_error,
        packet.state.measuring_completed,
        packet.state.measuring_aborted,

        packet.state.system_in_fault_condition,

        packet.state.save_measurements_running,
        packet.state.save_measurements_completed,
        packet.state.save_measurements_error,

        packet.state.data_transfer_read,
    };

    uint8_t* data_p = &outputData[15];
    add_rfUint32_to_packet(*(uint32_t*)(void*)&packet.result.diameter_min, &data_p, kEndianessBig);
    add_rfUint32_to_packet(*(uint32_t*)(void*)&packet.result.diameter_max, &data_p, kEndianessBig);
    add_rfUint32_to_packet(*(uint32_t*)(void*)&packet.result.diameter_avg, &data_p, kEndianessBig);

    add_rfUint32_to_packet(*(uint32_t*)(void*)&packet.result.chamfer_width_min, &data_p, kEndianessBig);
    add_rfUint32_to_packet(*(uint32_t*)(void*)&packet.result.chamfer_width_max, &data_p, kEndianessBig);
    add_rfUint32_to_packet(*(uint32_t*)(void*)&packet.result.chamfer_width_avg, &data_p, kEndianessBig);

    add_rfUint32_to_packet(*(uint32_t*)(void*)&packet.result.circularity_min, &data_p, kEndianessBig);
    add_rfUint32_to_packet(*(uint32_t*)(void*)&packet.result.circularity_max, &data_p, kEndianessBig);
    add_rfUint32_to_packet(*(uint32_t*)(void*)&packet.result.circularity_avg, &data_p, kEndianessBig);

    add_rfUint32_to_packet(*(uint32_t*)(void*)&packet.result.bevel_angle_min, &data_p, kEndianessBig);
    add_rfUint32_to_packet(*(uint32_t*)(void*)&packet.result.bevel_angle_max, &data_p, kEndianessBig);
    add_rfUint32_to_packet(*(uint32_t*)(void*)&packet.result.bevel_angle_avg, &data_p, kEndianessBig);

    add_rfUint32_to_packet(*(uint32_t*)(void*)&packet.result.band_height_min, &data_p, kEndianessBig);
    add_rfUint32_to_packet(*(uint32_t*)(void*)&packet.result.band_height_max, &data_p, kEndianessBig);
    add_rfUint32_to_packet(*(uint32_t*)(void*)&packet.result.band_height_avg, &data_p, kEndianessBig);

    if (!isInit)
        return false;

    std::cout << std::endl;
    std::cout << "Start sending..." << std::endl;

    if (!channel->Send_Data(outputData, sizeof (outputData)))
    {
        std::cout << "Data not sent" << std::endl;
        return false;
    }

    return true;

}

bool CamsoWorker::SendData(plc_packet packet)
{
    uint8_t outputData[15] = {
        packet.calibration_start,
        packet.zero_positioning_start,
        packet.measuring_start,
        packet.abort_measure,
        packet.fault_reset,
        packet.save_measurements,
        packet.new_data_transfer,
    };

    uint8_t* data_p = &outputData[7];
    add_rfUint32_to_packet(*(uint32_t*)(void*)&packet.scan_length, &data_p, kEndianessBig);
    add_rfUint32_to_packet(*(uint32_t*)(void*)&packet.calibration_diameter, &data_p, kEndianessBig);

    if (!isInit)
        return false;

    std::cout << std::endl;
    std::cout << "Start sending..." << std::endl;

    if (!channel->Send_Data(outputData, sizeof (outputData)))
    {
        std::cout << "Data not sent" << std::endl;
        return false;
    }

    return true;

}

void CamsoWorker::ReadThreadFuncPLC()
{
    if (isInit)
    {

        isRun = true;

        // Allocate memory for data.
        uint8_t inputData[15];

        std::cout << std::endl;
        std::cout << "Start reading..." << std::endl;
        emit Print("Start reading...\n");

        // Main loop.
        while (isRun)
        {
            Sleep(10);
            // Wait data.
            uint32_t inputDataSize = 0;
            if (!channel->Get_Data(inputData, 15, inputDataSize, 0, m_TimeoutMs))
            {
                std::cout << "No input data" << std::endl;
                emit Print("No input data\n");
                continue;
            }

            std::cout << "port " << (int32_t)m_SerialPort << "  " << inputDataSize << " bytes" << std::endl;
            emit Print("Get data from port " + QString::number(m_SerialPort) + " " + QString::number(inputDataSize) + " bytes\n");

            emit GetData((char*)&inputData[0], inputDataSize);

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

            bool save_measurements = (bool)inputData[5];
            if (save_measurements)
            {
                emit SaveMeasurements();
            }

            bool new_data_transfer = (bool)inputData[6];
            if (new_data_transfer)
            {
                uint8_t* data_p = &inputData[7];
                uint32_t a = get_rfUint32_from_packet(&data_p, kEndianessBig);
                float resA = *(float*)(uint32_t*)(&a);

                uint32_t b = get_rfUint32_from_packet(&data_p, kEndianessBig);
                float resB = *(float*)(uint32_t*)(&b);
                emit NewDataTransfer(resA, resB);
            }
        }
    }
    emit finished();

}

void CamsoWorker::ReadThreadFuncSystem()
{
    if (isInit)
    {

        isRun = true;

        // Allocate memory for data.
        uint8_t inputData[75];

        std::cout << std::endl;
        std::cout << "Start reading..." << std::endl;
        emit Print("Start reading...\n");

        // Main loop.
        while (isRun)
        {
            Sleep(m_TimeoutMs/2);
            // Wait data.
            uint32_t inputDataSize = 0;
            if (!channel->Get_Data(inputData, 75, inputDataSize, 0, m_TimeoutMs/2))
            {
                std::cout << "No input data" << std::endl;
                emit Print("No input data\n");
                continue;
            }


            std::cout << "port " << (int32_t)m_SerialPort << "  " << inputDataSize << " bytes" << std::endl;
            emit Print("Get data from port " + QString::number(m_SerialPort) + " " + QString::number(inputDataSize) + " bytes\n");

            emit GetData((char*)&inputData[0], inputDataSize);

        }
    }
    emit finished();

}

void CamsoWorker::Stop()
{
    isRun = false;
    isInit = false;
}

bool CamsoWorker::IsWork()
{
    return isRun;
}

void CamsoWorker::SetPortTimeoutMs(uint32_t value)
{
    m_TimeoutMs = value;
}
