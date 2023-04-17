#pragma once
#include <mutex>
#include <thread>
#include <atomic>
#include <chrono>
#include <vector>
#include "Channel.h"
#include "SerialPort.h"
#include "DefaultParser.h"


namespace I7580
{
	/**
	 * @brief Serial port communication channel class.
	 */
	class SerialChannel : public Channel
	{
	public:

		/**
		 * @brief Class constructor.
		 */
		SerialChannel();

		/**
		 * @brief Class destructor.
		 */
		~SerialChannel();

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

		/// Vector of pointer to perser objects. This parsers are shared between SerialChannel instances.
        static std::vector<DefaultParser*> parser;
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
		/// Thread for communication.
		std::thread communication_thread;
		/// Flag to stop threads.
		std::atomic<bool> stop_threads_flag;
		/// Channel bandwidth.
		std::atomic<float> channel_bandwidth_Mbps;
		/// Conditional variable for new data.
		std::condition_variable output_data_cond_var;
		/// Mutex for conditional variable.
		std::mutex output_data_cond_var_mutex;
		/// Flag for conditional variable.
		std::atomic<bool> output_data_cond_var_flag;
		/// Boudrate.
		uint32_t baudrate;

		/**
		 * @brief Communication thread.
		*/
		void Communication_Thread_Function();

		/**
		* @brief Function to split string into parameters substring.
		*/
		template <class Container>
		void Split_String(const std::string& str, Container& cont, const char& delimited = ';', bool removeSpace = true);

	};

}

