#ifndef ENDIANCONV_H
#define ENDIANCONV_H

#include <stdint.h>
#include <stddef.h>

/** @file endianconv.h
 * @brief Responsible for Endianess conversion
 */

typedef enum {
    kEndianessUnknown = -1,
    kEndianessLittle = 0,
    kEndianessBig = 1
} rf_endianess;

/**
 * read/write arbitrary bits macros
 */
#define get_mask(index, size) \
    (((1 << (size)) - 1) << (index))
#define read_from(data, index, size) \
    (((data) & get_mask((index), (size))) >> (index))
#define write_to(data, index, size, value) \
    ((data) = ((data) & (~get_mask((index), (size)))) | ((value) << (index)))

/**
 * @brief get_rfInt8_from_packet. Reads rfInt8 from *buffer and
 *        converts little endian to host.
 * @param buffer - Pointer to the buffer array.
 *        This pointer will be incremented by 1;
 * @return Extracted 8 bit rfInteger value
 */
int8_t  get_rfInt8_from_packet (uint8_t** buffer_address);

/**
 * @brief get_rfUint8_from_packet. Reads rfUint8 from *buffer and
 *        converts little endian to host.
 * @param buffer - Pointer to the buffer array.
 *        This pointer will be incremented by 1;
 * @return Extracted 8 bit rfInteger value
 */
uint8_t get_rfUint8_from_packet(uint8_t** buffer_address);

/**
 * @brief get_rfChar_from_packet. Reads rfChar from *buffer_address and
 *        converts little endian to host.
 * @param buffer_address - Pointer to the buffer_address.
 *        This pointer will be incremented by 1;
 * @return Extracted 8 bit rfInteger value
 */
char  get_rfChar_from_packet (uint8_t** buffer_address);

/**
 * @brief get_rfByte_from_packet. Reads rfByte from *buffer_address and
 *        converts little endian to host.
 * @param buffer_address - Pointer to the buffer_address .
 *        This pointer will be incremented by 1;
 * @return Extracted 8 bit rfInteger value
 */
uint8_t  get_rfByte_from_packet (uint8_t** buffer_address);

/**
 * @brief get_rfBool_from_packet. Reads rfBool from *buffer_address and
 *        converts little endian to host.
 * @param buffer_address - Pointer to the buffer_address.
 *        This pointer will be incremented by 1;
 * @return Extracted 8 bit rfInteger value
 */
uint8_t  get_rfBool_from_packet (uint8_t** buffer_address);

/**
 * @brief get_rfInt16_from_packet. Reads rfInt16 from *buffer_address and
 *        converts little endian to host.
 * @param buffer_address - Pointer to the buffer_address.
 *        This pointer will be incremented by 2;
 * @return Extracted 16 bit rfInteger value
 */
int16_t  get_rfInt16_from_packet (uint8_t** buffer_address, rf_endianess endian);

/**
 * @brief get_rfUint16_from_packet. Reads rfUint16 from *buffer_address and
 *        converts little endian to host.
 * @param buffer_address - Pointer to the buffer_address.
 *        This pointer will be incremented by 2;
 * @return Extracted 16 bit rfInteger value
 */
uint16_t get_rfUint16_from_packet(uint8_t** buffer_address, rf_endianess endian);

/**
 * @brief get_rfInt32_from_packet. Reads rfInt32 from *buffer_address and
 *        converts little endian to host.
 * @param buffer_address - Pointer to the buffer_address.
 *        This pointer will be incremented by 4;
 * @return Extracted 32 bit rfInteger value
 */
int32_t  get_rfInt32_from_packet (uint8_t** buffer_address, rf_endianess endian);

/**
 * @brief get_rfUint32_from_packet. Reads rfUint32 from *buffer_address and
 *        converts little endian to host.
 * @param buffer_address - Pointer to the buffer_address.
 *        This pointer will be incremented by 4;
 * @return Extracted 32 bit rfInteger value
 */
uint32_t get_rfUint32_from_packet(uint8_t** buffer_address, rf_endianess endian);

/**
 * @brief get_array_from_packet. Reads num bytes from source_buffer to
 *        destination_buffer.
 * @param destination_buffer - Pointer to the destination buffer.
 *        This pointer will be incremented by Num;
 * @param source_buffer_address - Pointer to the source buffer_address
 * @param num - count of bytes to read from array
 * @return count of readed bytes
 */
size_t get_array_from_packet(
        uint8_t* destination_buffer, uint8_t** source_buffer_address, size_t num);


/**
 * @brief add_rfInt8_to_packet. Write an rfInt8 to the buffer.
 * @param data - value to be written
 * @param buffer_address - pointer to the buffer_address. This pointer will
 *        be incremented by 1!
 * @return Length in bytes of the encoded packet
 */
size_t add_rfInt8_to_packet (int8_t data, uint8_t** buffer_address);

/**
 * @brief add_rfUint8_to_packet. Write an rfUint8 to the buffer.
 * @param data - value to be written
 * @param buffer - buffer pointer to the buffer array. This pointer will
 *        be incremented by 1!
 * @return Length in bytes of the encoded packet
 */
size_t add_rfUint8_to_packet(uint8_t data, uint8_t** buffer_address);

/**
 * @brief add_rfChar_to_packet. Write an rfChar to the buffer.
 * @param data - value to be written
 * @param buffer - buffer pointer to the buffer array. This pointer will
 *        be incremented by 1!
 * @return Length in bytes of the encoded packet
 */
size_t add_rfChar_to_packet (char data, uint8_t** buffer_address);

/**
 * @brief add_rfByte_to_packet. Write an rfByte to the buffer.
 * @param data - value to be written
 * @param buffer - buffer pointer to the buffer array. This pointer will
 *        be incremented by 1!
 * @return Length in bytes of the encoded packet
 */
size_t add_rfByte_to_packet (uint8_t data, uint8_t** buffer_address);

/**
 * @brief add_rfBool_to_packet. Write an rfBool to the buffer.
 * @param data - value to be written
 * @param buffer - buffer pointer to the buffer array. This pointer will
 *        be incremented by 1!
 * @return Length in bytes of the encoded packet
 */
size_t add_rfBool_to_packet (uint8_t data, uint8_t** buffer_address);

/**
 * @brief add_rfInt16_to_packet. Write an rfInt16 to the buffer.
 * @param data - value to be written
 * @param buffer - buffer pointer to the buffer array. This pointer will
 *        be incremented by 2!
 * @return Length in bytes of the encoded packet
 */
size_t add_rfInt16_to_packet (
        int16_t data, uint8_t** buffer_address, rf_endianess endian);

/**
 * @brief add_rfUint16_to_packet. Write an rfUint16 to the buffer.
 * @param data - value to be written
 * @param buffer - buffer pointer to the buffer array. This pointer will
 *        be incremented by 2!
 * @return Length in bytes of the encoded packet
 */
size_t add_rfUint16_to_packet(
        uint16_t data, uint8_t** buffer_address, rf_endianess endian);

/**
 * @brief add_rfrfInt32o_packet. Write an rfInt32 to the buffer.
 * @param data - value to be written
 * @param buffer - buffer pointer to the buffer array. This pointer will
 *        be incremented by 4!
 * @return Length in bytes of the encoded packet
 */
size_t add_rfInt32_to_packet (
        int32_t data, uint8_t** buffer_address, rf_endianess endian);

/**
 * @brief add_rfrfUint32o_packet. Write an rfUint32 to the buffer.
 * @param data - value to be written
 * @param buffer - buffer pointer to the buffer array. This pointer will
 *        be incremented by 4!
 * @return Length in bytes of the encoded packet
 */
size_t add_rfUint32_to_packet(
        uint32_t data, uint8_t** buffer_address, rf_endianess endian);



#ifdef _SUPPORT_64BIT_DATATYPES

/**
 * @brief get_rfUint64_from_packet. Reads rfUint64 from *buffer and
 *        converts little endian to host.
 * @param buffer - Pointer to the buffer array.
 *        This pointer will be incremented by 8;
 * @return Extracted 32 bit rfInteger value
 */
rfUint64 get_rfUint64_from_packet(rfUint8** buffer_address, rf_endianess endian);

/**
 * @brief get_rfUint64_from_packet. Reads rfUint64 from *buffer and
 *        converts little endian to host.
 * @param buffer - Pointer to the buffer array.
 *        This pointer will be incremented by 8;
 * @return Extracted 32 bit rfInteger value
 */
rfUint64 get_rfInt64_from_packet(rfUint8** buffer_address, rf_endianess endian);

/**
 * @brief add_rfUint64_to_packet. Write an rfUint64 to the buffer.
 * @param data - value to be written
 * @param buffer - buffer pointer to the buffer array. This pointer will
 *        be incremented by 8!
 * @return Length in bytes of the encoded packet
 */
rfSize add_rfUint64_to_packet(
        rfUint64 data, rfUint8** buffer_address, rf_endianess endian);

/**
 * @brief add_rfUint64_to_packet. Write an rfUint64 to the buffer.
 * @param data - value to be written
 * @param buffer - buffer pointer to the buffer array. This pointer will
 *        be incremented by 8!
 * @return Length in bytes of the encoded packet
 */
rfSize add_rfInt64_to_packet(
        rfInt64 data, rfUint8** buffer_address, rf_endianess endian);

#endif



/** Identify if we are running on a big or little endian system and set
 * variable.
 */
void determine_endianess(void);

/** @brief Return the endianess identified on system startup
 * @return
 *    - -1 endianess has not been identified up to now
 *    - 0  little endian system
 *    - 1  big endian system
 */
int8_t get_endianess(void);


size_t move_packet_n_bytes(
        int n, uint8_t** packet_runner);

size_t fill_next_n_packet_bytes_with(
        uint8_t value, uint32_t n, uint8_t** packet);

size_t fill_next_n_packet_bytes_with_value_and_move_to_next_position(
        uint8_t value, uint32_t n, uint8_t** packet);

#endif // ENDIANCONV_H
