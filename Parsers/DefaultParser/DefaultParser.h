#pragma once
#include <atomic>
#include <mutex>
#include <condition_variable>


/*
STP - Serial Transport Protocol.
DATA packets format:
	uint8_t  0xAA (start byte)
	uint8_t  0xA0 | buffer_ID(4 bits)
	uint8_t  logic_port
	uint16_t packet_ID & 0x0FFF
	uint16_t max_packet_ID & 0x0FFF
	uint16_t packet_size
	uint8_t  header_CRC
	uint8_t  data[...]
*/


namespace I7580
{
	/**
	 * @brief Network Transport Protocol Parser Class.
	*/
	class DefaultParser
	{
	public:

		/**
		 * @brief Class constructor.
		*/
		DefaultParser();

		/**
		 * @brief Class destructor.
		*/
		~DefaultParser();

		/**
		 * @brief Method to add data to send.
		 * @param data Pointer to data to send.
		 * @param data_size Size of data.
		 * @param logic_port Logic port [0:15].
		 * @param number_of_repetitions Number of send cycles.
		 * @return TRUE if data added or FALSE.
		*/
		bool Add_Data_To_Send(uint8_t* data, uint32_t data_size, uint8_t logic_port, uint8_t number_of_repetitions = 2);

		/**
		 * @brief Method to get next packet to send.
		 * @param packet_data Pointer to buffer of packet data to fill.
		 * @param packet_size Size of packet.
		 * @return TRUE if new packet was copied of FALSE if there are no packets.
		*/
		bool Get_Next_Output_Packet(uint8_t* packet_data, uint16_t& packet_size);

		/**
		 * @brief Method to decode input packet.
		 * @param next_byte Next byte to process.
		 * @return ( -1) - error in packet data;
		           ( -3) - no data;
		           ( 0 ) - data ready;
				   ( 1 ) - DATA packet processed;
				   ( 2 ) - PACKET_CONFIRMATION packet processed.
		*/
		int32_t Decode_Serial_Data(uint8_t next_byte);

		/**
		 * @brief Method to get data. You can get next data only once.
		 * @param data_buffer Pointer to data buffer to fill.
		 * @param data_buffer_size Size of data buffer.
		 * @param input_data_size Size of input data.
		 * @param logic_port Logic port [0:15].
		 * @param timeout 0 - just check new data, -1 - wait data endlessly, >0 - wait timeout.
		 * @return TRUE if new data was copied of FALSE.
		*/
		bool Get_Data(uint8_t* data_buffer, uint32_t data_buffer_size, uint32_t& input_data_size, uint8_t logic_port, int32_t timeout = 0);

	public:

		uint8_t** output_packet[16];						// Buffer of output packets.
		std::atomic<uint16_t> output_max_packet_ID[16];		// MAximum ID of output data.
		uint16_t* output_packet_size[16];					// Size of packets.
		std::atomic<int32_t>* output_packet_counter[16];	// Counter for each packet.
		std::atomic<int32_t> output_packets_left[16];		// Counter of output packets left.
		uint16_t output_packet_index[16];					// Index of packet to send.
		uint8_t output_read_index;							// Index of buffer to send data.
		uint8_t output_add_index;							// Index of buffer to add new data.
		std::mutex output_data_mutex[16];					// Mutex to protect packets buffers.

		uint8_t* input_data[16];							// Buffers for input data.
		uint32_t input_data_size[16];						// Size of input data.
		uint32_t input_max_packet_ID[16];					// Maximum packet ID of input data.
		uint8_t* input_packet_flag[16];						// Flag of received packets.
		uint16_t input_packets_counter[16];					// Counter of input packets.

		uint8_t* ready_data[256];							// Pointer to ready input data.
		std::atomic<uint32_t> ready_data_size[256];			// Size of input ready data.
		std::mutex ready_data_mutex[256];					// Mutex to protect ready data.
		std::condition_variable ready_data_cond_var[256];	// Conditional wariable to notyficate about ready data.
		std::mutex ready_data_cond_var_mutex[256];			// Mutex to protect conditional variable.
		std::atomic<bool> ready_data_cond_var_flag[256];	// Flag for conditional variable.

		uint8_t packet_header_buffer[10];					/// Buffer for header of data packets.
		uint8_t* data_packet_buffer;						/// Buffer for packet data.
		uint32_t packet_data_pos;							/// Current position in serial packet.
		uint16_t serial_packet_size;						/// Size of packet.
		bool data_packet_flag;								/// Flag of DATA packet.

	};
}
