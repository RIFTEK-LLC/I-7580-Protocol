#include "DefaultParser.h"
#include <cstring>


constexpr uint32_t max_packet_size = 504;
constexpr uint32_t max_payload_size = max_packet_size - 10;
constexpr uint32_t max_data_size = 4096 * max_payload_size - 1;


I7580::DefaultParser::DefaultParser() :
	output_read_index(0),
	output_add_index(0),
	data_packet_buffer(nullptr),
	packet_data_pos(0),
	serial_packet_size(0),
	data_packet_flag(false)
{
	data_packet_buffer = new uint8_t[max_packet_size];
	memset(data_packet_buffer, 0, max_packet_size);
	memset(packet_header_buffer, 0, 10);

	for (uint32_t i = 0; i < 16; ++i)
	{
		output_packet[i] = new uint8_t * [4096];
		output_packet_size[i] = new uint16_t[4096];
		output_packet_counter[i] = new std::atomic<int32_t>[4096];

		for (uint32_t j = 0; j < 4096; ++j)
		{
			output_packet[i][j] = new uint8_t[max_packet_size];
			memset(output_packet[i][j], 0, max_packet_size);
			output_packet_size[i][j] = 0;
			output_packet_counter[i][j] = 0;
		}

		output_max_packet_ID[i] = 0;
		output_packets_left[i] = 0;
		output_packet_index[i] = 0;

		input_data[i] = new uint8_t[max_data_size];
		memset(input_data[i], 0, max_data_size);
		input_data_size[i] = 0;
		input_max_packet_ID[i] = 0;
		input_packet_flag[i] = new uint8_t[4096];
		for (uint32_t j = 0; j < 4096; ++j)
			input_packet_flag[i][j] = 0;
		input_packets_counter[i] = 0;
	}

	for (uint32_t i = 0; i < 256; ++i)
	{
		ready_data[i] = nullptr;
		ready_data_size[i] = 0;
		ready_data_cond_var_flag[i] = false;
	}
}


I7580::DefaultParser::~DefaultParser()
{
	for (uint32_t i = 0; i < 16; ++i)
	{
		for (uint32_t j = 0; j < 4096; ++j)
			delete[] output_packet[i][j];

		delete[] output_packet[i];
		delete[] output_packet_size[i];
		delete[] output_packet_counter[i];

		delete[] input_packet_flag[i];

		delete[] input_data[i];
	}

	for (uint32_t i = 0; i < 256; ++i)
	{
		if (ready_data[i] != nullptr)
			delete[] ready_data[i];
	}

	delete[] data_packet_buffer;
}


bool I7580::DefaultParser::Add_Data_To_Send(uint8_t* data, uint32_t data_size, uint8_t logic_port, uint8_t number_of_repetitions)
{
	if (data_size == 0 || data_size > max_data_size)
		return false;

	output_data_mutex[output_add_index].lock();

	output_max_packet_ID[output_add_index] = data_size / max_payload_size;
	if (data_size % max_payload_size == 0)
		output_max_packet_ID[output_add_index] = output_max_packet_ID[output_add_index] - 1;
	output_packet_index[output_add_index] = 4095;

	output_packets_left[output_add_index] = ((int32_t)output_max_packet_ID[output_add_index] + 1);

	for (uint32_t i = 0; i < 4096; ++i)
	{
		output_packet_size[output_add_index][i] = 0;
		output_packet_counter[output_add_index][i] = 0;
	}

	uint8_t first_byte = 0xA0 | output_add_index;

	for (uint16_t i = 0; i <= output_max_packet_ID[output_add_index]; ++i)
	{
		uint32_t data_pos = (uint32_t)i * max_payload_size;
		uint32_t payload_size = data_size - data_pos;
		if (payload_size > max_payload_size)
			payload_size = max_payload_size;

		output_packet_size[output_add_index][i] = payload_size + 10;

		output_packet[output_add_index][i][0] = 0xAA;
		output_packet[output_add_index][i][1] = first_byte;
		output_packet[output_add_index][i][2] = logic_port;
		output_packet[output_add_index][i][3] = (uint8_t)(i >> 8);
		output_packet[output_add_index][i][4] = (uint8_t)i;
		output_packet[output_add_index][i][5] = (uint8_t)(output_max_packet_ID[output_add_index] >> 8);
		output_packet[output_add_index][i][6] = (uint8_t)output_max_packet_ID[output_add_index];
		output_packet[output_add_index][i][7] = (uint8_t)(output_packet_size[output_add_index][i] >> 8);
		output_packet[output_add_index][i][8] = (uint8_t)output_packet_size[output_add_index][i];
		output_packet[output_add_index][i][9] = (uint8_t)(
			output_packet[output_add_index][i][0] +
			output_packet[output_add_index][i][1] +
			output_packet[output_add_index][i][2] +
			output_packet[output_add_index][i][3] +
			output_packet[output_add_index][i][4] +
			output_packet[output_add_index][i][5] +
			output_packet[output_add_index][i][6] +
			output_packet[output_add_index][i][7] +
			output_packet[output_add_index][i][8]);

		memcpy(&output_packet[output_add_index][i][10], &data[data_pos], payload_size);

		output_packet_counter[output_add_index][i] = 1;
	}

	output_data_mutex[output_add_index].unlock();

	++output_add_index;
	output_add_index = output_add_index & 0x0F;

	return true;
}


bool I7580::DefaultParser::Get_Next_Output_Packet(uint8_t* packet_data, uint16_t& packet_size)
{
	packet_size = 0;

	for (uint32_t i = 0; i < 16; ++i)
	{
		output_data_mutex[output_read_index].lock();

		if (output_packets_left[output_read_index] > 0)
		{
			for (uint16_t j = 0; j <= output_max_packet_ID[output_read_index]; ++j)
			{
				++output_packet_index[output_read_index];
				if (output_packet_index[output_read_index] > output_max_packet_ID[output_read_index])
					output_packet_index[output_read_index] = 0;

				if (output_packet_counter[output_read_index][output_packet_index[output_read_index]] > 0)
				{
					packet_size = output_packet_size[output_read_index][output_packet_index[output_read_index]];
					memcpy(packet_data, output_packet[output_read_index][output_packet_index[output_read_index]], packet_size);

					output_packet_counter[output_read_index][output_packet_index[output_read_index]]--;
					output_packets_left[output_read_index]--;

					output_data_mutex[output_read_index].unlock();

					return true;
				}
			}
		}

		output_data_mutex[output_read_index].unlock();

		++output_read_index;
		output_read_index = output_read_index & 0x0F;
	}

	return false;
}


bool I7580::DefaultParser::Get_Data(uint8_t* data_buffer, uint32_t data_buffer_size, uint32_t& input_data_size, uint8_t logic_port, int32_t timeout)
{
	if (ready_data_size[logic_port] > 0)
	{
		ready_data_mutex[logic_port].lock();

		if (data_buffer_size < ready_data_size[logic_port])
		{
			ready_data_cond_var_flag[logic_port] = false;
			ready_data_mutex[logic_port].unlock();
			return false;
		}

		memcpy(data_buffer, ready_data[logic_port], ready_data_size[logic_port]);
		input_data_size = ready_data_size[logic_port];

		ready_data_size[logic_port] = 0;
		ready_data_cond_var_flag[logic_port] = false;

		ready_data_mutex[logic_port].unlock();

		return true;
	}

	if (timeout == 0)
		return false;

	if (timeout < 0)
	{
		std::unique_lock<std::mutex> lk(ready_data_cond_var_mutex[logic_port]);
		while (!ready_data_cond_var_flag[logic_port])
			ready_data_cond_var[logic_port].wait(lk);
		ready_data_cond_var_flag[logic_port] = false;
		lk.unlock();
	}
	else
	{
		std::unique_lock<std::mutex> lk(ready_data_cond_var_mutex[logic_port]);
		while (!ready_data_cond_var_flag[logic_port])
			if (ready_data_cond_var[logic_port].wait_for(lk, std::chrono::milliseconds(timeout)) == std::cv_status::timeout)
				return false;
		ready_data_cond_var_flag[logic_port] = false;
		lk.unlock();
	}

	if (ready_data_size[logic_port] > 0)
	{
		ready_data_mutex[logic_port].lock();

		if (data_buffer_size < ready_data_size[logic_port])
		{
			ready_data_cond_var_flag[logic_port] = false;
			ready_data_mutex[logic_port].unlock();
			return false;
		}

		memcpy(data_buffer, ready_data[logic_port], ready_data_size[logic_port]);
		input_data_size = ready_data_size[logic_port];

		ready_data_size[logic_port] = 0;
		ready_data_cond_var_flag[logic_port] = false;

		ready_data_mutex[logic_port].unlock();

		return true;
	}

	return false;
}


int32_t I7580::DefaultParser::Decode_Serial_Data(uint8_t next_byte)
{
	if (data_packet_flag == false)
	{
		for (uint32_t i = 1; i < 10; ++i)
			packet_header_buffer[i - 1] = packet_header_buffer[i];
		packet_header_buffer[9] = next_byte;

		if (packet_header_buffer[0] != 0xAA)
			return -2;

		uint8_t checksum = 0;
		checksum += packet_header_buffer[0];
		checksum += packet_header_buffer[1];
		checksum += packet_header_buffer[2];
		checksum += packet_header_buffer[3];
		checksum += packet_header_buffer[4];
		checksum += packet_header_buffer[5];
		checksum += packet_header_buffer[6];
		checksum += packet_header_buffer[7];
		checksum += packet_header_buffer[8];

		// Check checksum.
		if (checksum != packet_header_buffer[9])
			return -2;

		// Check DATA packet header.
		if ((packet_header_buffer[1] & 0xF0) != 0xA0)
			return -2;

		// Get packet size.
		uint16_t packet_size = (uint16_t)packet_header_buffer[7] * 256 + (uint16_t)packet_header_buffer[8];

		// Check packet size.
		if (packet_size > max_packet_size || packet_size <= 10)
			return -2;

		// Init data atributes.
		serial_packet_size = packet_size;
		data_packet_flag = true;
		packet_data_pos = 10;

		// Copy header data.
		memcpy(data_packet_buffer, packet_header_buffer, 10);

		return -2;
	}

	// Add byte to data packet.
	data_packet_buffer[packet_data_pos] = next_byte;
	++packet_data_pos;

	// Check if packet ready.
	if (packet_data_pos < serial_packet_size)
		return -2;

	// Reset header data buffer.
	memset(packet_header_buffer, 0, 10);
	data_packet_flag = false;

	uint8_t buffer_ID = data_packet_buffer[1] & 0x0F;
	uint8_t logic_port = data_packet_buffer[2];

	uint16_t packet_ID = (uint16_t)data_packet_buffer[3] * 256 + (uint16_t)data_packet_buffer[4];
	packet_ID = packet_ID & 0x0FFF;
	uint16_t max_packet_ID = (uint16_t)data_packet_buffer[5] * 256 + (uint16_t)data_packet_buffer[6];
	max_packet_ID = max_packet_ID & 0x0FFF;

	if (packet_ID > max_packet_ID)
		return -1;

	if (input_packets_counter[buffer_ID] == 0)
	{
		uint8_t previous_buffer_ID = (uint8_t)(buffer_ID - 1) & 0x0F;
		input_data_size[previous_buffer_ID] = 0;
		input_max_packet_ID[previous_buffer_ID] = 0;
		input_packets_counter[previous_buffer_ID] = 0;

		input_max_packet_ID[buffer_ID] = max_packet_ID;
		memset(input_packet_flag[buffer_ID], 1, 4096);
		input_data_size[buffer_ID] = 0;
	}

	memcpy(&input_data[buffer_ID][(uint32_t)packet_ID * max_payload_size], &data_packet_buffer[10], serial_packet_size - 10);

	input_packets_counter[buffer_ID] += (uint32_t)input_packet_flag[buffer_ID][packet_ID];
	input_packet_flag[buffer_ID][packet_ID] = 0;

	if (packet_ID == max_packet_ID)
		input_data_size[buffer_ID] = max_packet_ID * max_payload_size + serial_packet_size - 10;

	if (input_packets_counter[buffer_ID] > input_max_packet_ID[buffer_ID])
	{
		input_packets_counter[buffer_ID] = 0;

		ready_data_mutex[logic_port].lock();
		ready_data_size[logic_port] = input_data_size[buffer_ID];
		if (ready_data[logic_port] == nullptr)
			ready_data[logic_port] = new uint8_t[max_data_size];
		memcpy(ready_data[logic_port], input_data[buffer_ID], input_data_size[buffer_ID]);
		ready_data_mutex[logic_port].unlock();

		std::unique_lock<std::mutex> lk(ready_data_cond_var_mutex[logic_port]);
		ready_data_cond_var_flag[logic_port] = true;
		ready_data_cond_var[logic_port].notify_one();
		lk.unlock();

		return 0;
	}

	return 1;
}