#include <iostream>
#include <chrono>
#include <ctime>
#include <CamsoChannel.h>

// Global variables.
int32_t serial_port = 14;
uint32_t bandwidth = 115200;

void SendThreadFunc();

int main(void)
{
	// Dialog to enter serial port number.
    std::cout << "Enter first serial port number : ";
    std::cin >> serial_port;

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
    I7580::Channel* channel = new I7580::CamsoChannel();
    std::string initString = "\\\\.\\COM" + std::to_string(serial_port) + ";"
                                          + std::to_string(bandwidth);
	if (!channel->Init(initString))
	{
		std::cout << "Output channel not init" << std::endl;
		return;
	}

    bool calibration_start       = false;
    bool zero_positioning_start  = true;
    bool measuring_start         = false;
    bool abort_measure           = true;
    bool fault_reset             = false;
    bool save_measurements       = true;
    bool new_data_transfer       = false;

    float scan_length              = 10;
    float calibration_diameter     = 200;

    uint8_t outputData[15] = {
        calibration_start,
        zero_positioning_start,
        measuring_start,
        abort_measure,
        fault_reset,
        save_measurements,
        new_data_transfer
    };

    memcpy(&outputData[7], (uint8_t*)&scan_length, sizeof(float));
    memcpy(&outputData[11], (uint8_t*)&calibration_diameter, sizeof(float));

	std::cout << std::endl;
	std::cout << "Start sending..." << std::endl;

    while (true) {
        if (!channel->Send_Data(outputData, sizeof (outputData)))
            std::cout << "Data not sent" << std::endl;
    }

}
