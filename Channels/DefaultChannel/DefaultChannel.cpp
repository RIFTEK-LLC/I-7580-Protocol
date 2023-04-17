#include "DefaultChannel.h"
#include <algorithm>
#include <sstream>


std::vector<I7580::DefaultParser*> I7580::SerialChannel::parser;
std::vector<I7580::SerialPort*> I7580::SerialChannel::serial_port;
std::vector<std::string> I7580::SerialChannel::serial_port_name;
std::vector<std::mutex*> I7580::SerialChannel::instance_mutex;
std::vector<std::mutex*> I7580::SerialChannel::output_serial_port_mutex;
std::mutex I7580::SerialChannel::global_mutex;

// Global variables.
constexpr uint16_t default_read_timeout = 100;
constexpr uint32_t default_baudrate = 115200;
constexpr uint32_t default_max_packet_size = 1024;


/**
 * @brief Function to split string into parameters substring.
 */
template <class Container>
void I7580::SerialChannel::Split_String(const std::string& str, Container& cont, const char& delimited, bool removeSpace)
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


I7580::SerialChannel::SerialChannel() :
	instance_index(-1),
	stop_threads_flag(true),
	output_data_cond_var_flag(false),
	baudrate(0)
{

}


I7580::SerialChannel::~SerialChannel()
{
	// Stop reading thread
	stop_threads_flag = true;
	if (communication_thread.joinable())
		communication_thread.join();
}


bool I7580::SerialChannel::Init(std::string& init_string)
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
	for (size_t i = 0; i < parser.size(); ++i)
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
	DefaultParser* tmpProtocolParser = new DefaultParser();
	SerialPort* tmpSerialPort = new SerialPort();

	// Try open serial port.
	if (!tmpSerialPort->Open(portName.c_str(), tmpBaudrate))
	{
		delete tmpProtocolParser;
		delete tmpSerialPort;
		return false;
	}

	// Remember channel bandwidth.
	channel_bandwidth_Mbps = (float)tmpBaudrate / 1000000.0f;

	// Remember instance mutex.
	instance_index = (int32_t)parser.size();

	// Add objects to mutexes.
	parser.push_back(tmpProtocolParser);
	serial_port.push_back(tmpSerialPort);
	serial_port_name.push_back(portName);
	instance_mutex.push_back(new std::mutex());
	output_serial_port_mutex.push_back(new std::mutex());

	// Stop threads.
	stop_threads_flag = true;
	if (communication_thread.joinable())
		communication_thread.join();

	// Start thread.
	stop_threads_flag.store(false);
	communication_thread = std::thread(&SerialChannel::Communication_Thread_Function, this);

	return true;
}


bool I7580::SerialChannel::Send_Data(
	uint8_t* data,
	uint32_t data_size,
	uint8_t logic_port,
	uint32_t number_of_repetitions,
	float empty_param)
{

	// Check initialization
	if (instance_index < 0)
		return false;

	// Lock instance.
	std::lock_guard<std::mutex> safeLock(*instance_mutex[instance_index]);


	if (!parser[instance_index]->Add_Data_To_Send(data, data_size, logic_port, number_of_repetitions))
		return false;

    uint8_t* output_data = new uint8_t[default_max_packet_size];
    uint16_t output_size = 0;

    std::chrono::time_point<std::chrono::system_clock> slot_start_time = std::chrono::system_clock::now();
    while (!stop_threads_flag && parser[instance_index]->Get_Next_Output_Packet(output_data, output_size) == true)
    {
        serial_port[instance_index]->SendData(output_data, output_size);

        uint32_t time_slot_microseconds = 0;
        uint32_t bandwidth = channel_bandwidth_Mbps;
        if (bandwidth > 0)
            time_slot_microseconds = (default_max_packet_size * 8) / bandwidth;

        while (time_slot_microseconds > (uint32_t)std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::system_clock::now() - slot_start_time).count())

        slot_start_time = std::chrono::system_clock::now();
    }
//	std::unique_lock<std::mutex> lk(output_data_cond_var_mutex);
//	output_data_cond_var_flag = true;
//	output_data_cond_var.notify_one();
//	lk.unlock();

    delete[] output_data;
	return true;
}


bool I7580::SerialChannel::Get_Data(
	uint8_t* data_buffer,
	uint32_t data_buffer_size,
	uint32_t& input_data_size,
	uint8_t logic_port,
	int32_t timeout_Ms)
{
	// Check initialization.
	if (instance_index < 0)
		return false;

	// Lock
	std::lock_guard<std::mutex> safeLock(*instance_mutex[instance_index]);

	// Get or wait new input data.
	return parser[instance_index]->Get_Data(data_buffer, data_buffer_size, input_data_size, logic_port, timeout_Ms);
}


void I7580::SerialChannel::Communication_Thread_Function()
{
	uint8_t* input_data = new uint8_t[default_max_packet_size];
	int32_t input_size = 0;

	uint8_t* output_data = new uint8_t[default_max_packet_size];
	uint16_t output_size = 0;

	while (!stop_threads_flag)
	{

        input_size = serial_port[instance_index]->ReadData(input_data, default_max_packet_size);

        if (input_size > 0)
            for (int32_t i = 0; i < input_size; ++i)
                parser[instance_index]->Decode_Serial_Data(input_data[i]);

		std::chrono::time_point<std::chrono::system_clock> slot_start_time = std::chrono::system_clock::now();
		while (!stop_threads_flag && parser[instance_index]->Get_Next_Output_Packet(output_data, output_size) == true)
		{
			serial_port[instance_index]->SendData(output_data, output_size);

			uint32_t time_slot_microseconds = 0;
			uint32_t bandwidth = channel_bandwidth_Mbps;
			if (bandwidth > 0)
				time_slot_microseconds = (default_max_packet_size * 8) / bandwidth;

			while (time_slot_microseconds > (uint32_t)std::chrono::duration_cast<std::chrono::microseconds>(
                std::chrono::system_clock::now() - slot_start_time).count())

			slot_start_time = std::chrono::system_clock::now();
		}
	}
}
