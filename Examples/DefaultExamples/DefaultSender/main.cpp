#include <iostream>
#include <chrono>
#include <ctime>
#include <SerialChannel.h>

// Global constants.
constexpr uint32_t bandwidth = 115200;
constexpr uint8_t logic_port = 0;

std::atomic<uint32_t> time_point[256][256];
std::chrono::time_point<std::chrono::system_clock> start_time;
int32_t serial_port = 15;

void SendThreadFunc();

int main(void)
{
	// Dialog to enter serial port number.
    std::cout << "Enter first serial port number : ";
    //std::cin >> serial_port;

	// Run threads.
	std::thread sendThread(&SendThreadFunc);

	// Wait threads.
	if (sendThread.joinable())
		sendThread.join();

	return 1;
}


void SendThreadFunc()
{
	// Init channel.
	I7580::Channel* channel = new I7580::SerialChannel();
    std::string initString = "\\\\.\\COM" + std::to_string(serial_port) + ";" + std::to_string(bandwidth);
	if (!channel->Init(initString))
	{
		std::cout << "Output channel not init" << std::endl;
		return;
	}

    bool calibration_start = true;
    bool zero_positioning_start = true;
    bool measuring_start = true;
    bool abort_measure = true;
    bool fault_reset = true;
    bool new_data_transfer = true;

    int measuring_start_position = 10;
    int calibration_diameter = 200;

    uint8_t outputData[14] = {
        calibration_start,
        zero_positioning_start,
        measuring_start,
        abort_measure,
        fault_reset,
        new_data_transfer
    };

    memcpy(&outputData[6], (uint8_t*)&measuring_start_position, sizeof(int));
    memcpy(&outputData[10], (uint8_t*)&calibration_diameter, sizeof(int));

	std::cout << std::endl;
	std::cout << "Start sending..." << std::endl;

    while (true) {
        if (!channel->Send_Data(outputData, sizeof (outputData)))
            std::cout << "Data not sent" << std::endl;
    }

}
