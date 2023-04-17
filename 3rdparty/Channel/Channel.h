#pragma once
#include <stddef.h>
#include <stdint.h>
#include <string>


namespace I7580
{
	/**
	 * @brief Interface class for communication channels.
	 */
	class Channel
	{

	public:

		/**
		 * @brief Method to init communication channel.
		 * @param init_string Initialization string.
		 * @return TRUE in case of successful initialization or FALSE.
		 */
		virtual bool Init(std::string& init_string) = 0;

		/**
		 * @brief Method to send data.
		 * @param data Pointer to the data to send.
		 * @param data_size Size of data to send.
		 * @param logic_port Logic port number for data to send.
		 * @param number_of_repetitions Number of send repetitions.
		 * @param bandwidth_Mbps Bandwidth of communication channel in Mbit/s. Method controls
		 *                       sending bandwidth. If bandwidthMbps == 0 - the method will
		 *                       send data as fast as possible (this can lead to packet loss
		 *                       in the communication channel).
		 * @return TRUE if data was sent or FALSE.
		 */
		virtual bool Send_Data(
			uint8_t* data,
			uint32_t data_size,
			uint8_t logic_port = 0,
			uint32_t number_of_repetitions = 0,
            float bandwidth_Mbps = 0) = 0;

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
		virtual bool Get_Data(
			uint8_t* data_buffer,
			uint32_t data_buffer_size,
			uint32_t& input_data_size,
			uint8_t logic_port = 0,
			int32_t timeout_Ms = 0) = 0;

		/**
		 * @brief Virtual destructor.
		*/
		virtual ~Channel() {};

	};

}
