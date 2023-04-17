#include "CamsoChannel.h"
#include <algorithm>
#include <sstream>

std::vector<I7580::SerialPort*> I7580::CamsoChannel::serial_port;
std::vector<std::string> I7580::CamsoChannel::serial_port_name;
std::vector<std::mutex*> I7580::CamsoChannel::instance_mutex;
std::vector<std::mutex*> I7580::CamsoChannel::output_serial_port_mutex;
std::mutex I7580::CamsoChannel::global_mutex;

// Global variables.
constexpr uint32_t default_baudrate = 115200;
constexpr uint32_t default_max_packet_size = 376;



/**
 * @brief Function to split string into parameters substring.
 */
template <class Container>
void I7580::CamsoChannel::Split_String(const std::string& str, Container& cont, const char& delimited, bool removeSpace)
{
	std::string s = str;
	if (removeSpace)
		s.erase(std::remove(s.begin(), s.end(), ' '), s.end());
	std::stringstream ss(s);
	std::string token;
	while (std::getline(ss, token, delimited)) {
		cont.push_back(token);
	}
}


I7580::CamsoChannel::CamsoChannel() :
	instance_index(-1),
	baudrate(0)
{
}


I7580::CamsoChannel::~CamsoChannel()
{
}


bool I7580::CamsoChannel::Init(std::string& init_string)
{
	// Lock
	std::lock_guard<std::mutex> globalLock(global_mutex);

	// Get parameters value
	std::string portName = "";
	uint32_t tmpBaudrate = default_baudrate;


	try
	{
		// Get parameters from string
		std::vector<std::string> args;
		Split_String(init_string, args);

		switch (args.size())
		{
		case 1:
			portName = args[0];
			break;
		case 2:
			portName = args[0];
			tmpBaudrate = std::stoi(args[1]);
			break;
		default:
			return false;
		}
	}
	catch (const std::exception&)
	{
		return false;
	}

	// Find existing object with this parameters
	int32_t index = -1;
    for (size_t i = 0; i < serial_port_name.size(); ++i)
	{
		if (this->serial_port_name[i] == portName)
			index = (int32_t)i;
	}

	// Check if existing object found.
	if (index >= 0)
	{
		// Remember index.
		instance_index = index;

		return true;
	}

	// If we didn't finf existing object then create new one.
	SerialPort* tmpSerialPort = new SerialPort();

	// Try open serial port.
	if (!tmpSerialPort->Open(portName.c_str(), tmpBaudrate))
	{
		delete tmpSerialPort;
		return false;
	}

	// Remember instance mutex.
    instance_index = (int32_t)serial_port_name.size();

	// Add objects to mutexes.
	serial_port.push_back(tmpSerialPort);
	serial_port_name.push_back(portName);
	instance_mutex.push_back(new std::mutex());
	output_serial_port_mutex.push_back(new std::mutex());

	return true;
}


bool I7580::CamsoChannel::Send_Data(
        uint8_t* data,
        uint32_t data_size,
        uint8_t empty_param_1,
        uint32_t empty_param_2,
        float empty_param_3)
{

    // Check initialization
    if (instance_index < 0)
        return false;

    // Lock instance.
    std::lock_guard<std::mutex> safeLock(*instance_mutex[instance_index]);

    // Check data size.
    if (data_size == 0)
        return false;

    // Send data.
    if (serial_port[instance_index]->SendData(data, data_size) != data_size)
        return false;

    return true;
}


bool I7580::CamsoChannel::Get_Data(
        uint8_t* data_buffer,
        uint32_t data_buffer_size,
        uint32_t& input_data_size,
        uint8_t empty_param_1,
        int32_t timeout_Ms)
{
    // Check initialization.
    if (instance_index < 0)
        return false;

    // Lock
    std::lock_guard<std::mutex> safeLock(*instance_mutex[instance_index]);

    // Read data.
    int32_t num_input_bytes = 0;
    if (timeout_Ms == 0)
    {
        num_input_bytes = serial_port[instance_index]->ReadData(data_buffer, data_buffer_size);
        if (num_input_bytes <= 0)
            return false;

        input_data_size = (uint32_t)num_input_bytes;

        return true;
    }

    if (timeout_Ms < 0)
    {
        while (true)
        {
            num_input_bytes = serial_port[instance_index]->ReadData(data_buffer, data_buffer_size);
            if (num_input_bytes <= 0)
                continue;

            input_data_size = (uint32_t)num_input_bytes;

            return true;
        }
    }

    if (timeout_Ms > 0)
    {
        // Get start time point
        std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
        uint32_t readTime = 0;

        // Reading loop
        while (readTime < (uint32_t)timeout_Ms)
        {
            num_input_bytes = serial_port[instance_index]->ReadData(data_buffer, data_buffer_size);
            if (num_input_bytes <= 0)
            {
                // Update reading time
                readTime = (uint32_t)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start).count();
                continue;
            }

            input_data_size = (uint32_t)num_input_bytes;

            return true;
        }

        return false;
    }

    return false;
}
