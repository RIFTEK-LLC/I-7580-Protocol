#pragma once
#include <mutex>
#include <thread>
#include <atomic>
#include <chrono>
#include <vector>
#include "Channel.h"
#include "SerialPort.h"

#define CAMSO_MAX_PACKET_SIZE 376

namespace I7580
{
	/**
	 * @brief Serial port communication channel class.
	 */
    class CamsoChannel : public Channel
	{
	public:

		/**
		 * @brief Class constructor.
		 */
        CamsoChannel();

		/**
		 * @brief Class destructor.
		 */
        ~CamsoChannel();

		/**
		 * @brief Method to init communication channel.
		 * @param init_string Initialization string.
		 * @return TRUE in case of successful initialization or FALSE.
		 */
		bool Init(std::string& init_string);

		/**
		 * @brief Method to send data.
		 * @param data Pointer to the data to send.
		 * @param data_size Size of data to send.
		 * @param logic_port Logic port number for data to send.
		 * @param number_of_repetitions Number of send repetitions.
		 * @param empty_param Not used.
		 * @return TRUE if data was sent or FALSE.
		 */
		bool Send_Data(
			uint8_t* data,
			uint32_t data_size,
			uint8_t logic_port = 0,
			uint32_t number_of_repetitions = 0,
			float empty_param = 0);

		/**
		 * @brief Method to get input data.
		 * @param dataBuff Pointer to a buffer for copying data.
		 * @param dataBuffSize Size of buffer for copying data.
		 * @param inputSize Size of readed data.
		 * @param logicPort Logic port number of data the data to receive.
		 * @param timeoutMs Timeout in Ms to wait for data:
		 *                  timeoutMs == -1 - the method will wait indefinitely until data arrives;
		 *                  timeoutMs == 0  - the method will only check for new data;
		 *                  timeoutMs > 0   - the method will wait specified time.
		 * @return TRUE if data is and copied or FALSE if no new data or the data has already been read.
		 */
		bool Get_Data(
			uint8_t* data_buffer,
			uint32_t data_buffer_size,
			uint32_t& input_data_size,
			uint8_t logic_port = 0,
			int32_t timeout_Ms = 0);

	private:

		/// Vector of pointer to SerialPort objects. This objects are shared between SerialChannel instances.
		static std::vector<SerialPort*> serial_port;
		/// Vector of serial port names.
		static std::vector<std::string> serial_port_name;
		/// Vector of pointers to mutexes for protection of objects in vectors.
		static std::vector<std::mutex*> instance_mutex;
		/// Vector of pointers to mutexes for protection of objects in vectors.
		static std::vector<std::mutex*> output_serial_port_mutex;
		/// Global mutex to manage access to data in class constructor.
		static std::mutex global_mutex;
		/// Index of objects in vectors.
		int32_t instance_index;
		/// Boudrate.
		uint32_t baudrate;

		/**
		* @brief Function to split string into parameters substring.
		*/
		template <class Container>
		void Split_String(const std::string& str, Container& cont, const char& delimited = ';', bool removeSpace = true);

	};

}

