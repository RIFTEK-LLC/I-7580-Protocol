#include <iostream>
#include <chrono>
#include <ctime>
#include <SerialChannel.h>

// Global constants.
constexpr uint32_t bandwidth = 115200;
constexpr uint8_t logic_port_1 = 2;
constexpr uint8_t logic_port_2 = 8;

// Global variables.
constexpr double fps = 30;
std::atomic<uint32_t> time_point[256][256];
std::chrono::time_point<std::chrono::system_clock> start_time;
int32_t serial_port_1 = 0;
int32_t serial_port_2 = 0;

void SendThreadFunc();

void ReadThreadFunc();

int main(void)
{
	// Remember start time.
	start_time = std::chrono::system_clock::now();
	for (uint32_t i = 0; i < 256; ++i)
		for (uint32_t j = 0; j < 256; ++j)
			time_point[i][j] = 0;

	// Dialog to enter serial port number.
    std::cout << "Enter first serial port number : ";
    std::cin >> serial_port_1;

    std::cout << "Enter second serial port number : ";
    std::cin >> serial_port_2;

	// Run threads.
	std::thread sendThread(&SendThreadFunc);
    std::thread readThread(&ReadThreadFunc);

	// Wait threads.
	if (sendThread.joinable())
		sendThread.join();
    if (readThread.joinable())
        readThread.join();

	return 1;
}


void SendThreadFunc()
{
	// Init channel.
	I7580::Channel* channel = new I7580::SerialChannel();
	std::string initString = "\\\\.\\COM" + std::to_string(serial_port_1) + ";" + std::to_string(bandwidth);
	if (!channel->Init(initString))
	{
		std::cout << "Output channel not init" << std::endl;
		return;
	}

	float bandwidthMbps = (float)bandwidth / 1000000.0f;

	// Calculate data size.
	uint32_t normalDataSize = (uint32_t)(((double)bandwidthMbps * 1000000.0 / (fps * 8.0)) * 0.3);
	uint32_t maxDataSize = (uint32_t)((double)bandwidthMbps * 2.0 * 1000000.0 / (fps * 8.0));

	// Allocate memory for data.
    uint8_t* outputData = new uint8_t[maxDataSize];
    uint8_t dataValue = 0;

	std::cout << std::endl;
	std::cout << "Start sending..." << std::endl;

	// Main loop.
	uint8_t cycleCounter = 0;
	std::chrono::time_point<std::chrono::system_clock> startTime = std::chrono::system_clock::now();
	int32_t waitTime = 0;
	int32_t processingTime = 0;
	while (true)
	{
		++cycleCounter;
		uint32_t dataSizeToSend = normalDataSize;
		if (cycleCounter % 8 == 0)
			dataSizeToSend = maxDataSize;

		// Fill output data.
		memset(outputData, dataValue, dataSizeToSend);

		time_point[logic_port_1][dataValue] = (uint32_t)std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - start_time).count();

		++dataValue;

		// Send data.
		if (!channel->Send_Data(outputData, dataSizeToSend, logic_port_1, 3, bandwidthMbps))
			std::cout << "Data not sent" << std::endl;

		processingTime = (int32_t)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - startTime).count();
		waitTime = (int32_t)(1000.0 / fps) - processingTime - 1;
		if (waitTime > 0)
			std::this_thread::sleep_for(std::chrono::milliseconds(waitTime));
		startTime = std::chrono::system_clock::now();
	}
}

void ReadThreadFunc()
{
	// Init channel.
	I7580::Channel* channel = new I7580::SerialChannel();
	std::string initString = "\\\\.\\COM" + std::to_string(serial_port_2) + ";" + std::to_string(bandwidth);
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
	int32_t oldDataValue = 0;
	int32_t lostDataCounter = 0;
	uint8_t dataPartCounter = 0;
	while (true)
	{
		// Wait data.
		uint32_t inputDataSize = 0;
		if (!channel->Get_Data(inputData, 1048576, inputDataSize, logic_port_1, 1000))
		{
			std::cout << "No input data" << std::endl;
			continue;
		}

		// Get get time dellay.
		int32_t newDataValue = (int32_t)inputData[0];
		uint32_t time_dallay_mks = (uint32_t)std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - start_time).count() - time_point[logic_port_1][newDataValue];
		std::cout << "port " << (int32_t)logic_port_1 << "  " << inputDataSize << " bytes with " << time_dallay_mks << " mks latency" << std::endl;

		// Compare input data value.
		if (newDataValue - oldDataValue > 1)
		{
			lostDataCounter += newDataValue - oldDataValue - 1;
			//std::cout << lostDataCounter << " data lost" << std::endl;
		}
		else
		{
			if (newDataValue - oldDataValue != -255 && newDataValue < oldDataValue)
			{
				lostDataCounter += (255 - oldDataValue) + newDataValue;
				//std::cout << lostDataCounter << " data lost" << std::endl;
			}

		}
		oldDataValue = newDataValue;

		++dataPartCounter;
		if (dataPartCounter == 255)
		{
			//std::cout << "Recieved 256 data parts and lost " << lostDataCounter << " parts" << std::endl;
			lostDataCounter = 0;
		}
	}
}
