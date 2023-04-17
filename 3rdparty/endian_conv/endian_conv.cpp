#include "endian_conv.h"
#include <string.h>

rf_endianess g_platform_endianess = kEndianessUnknown;

/* THESE ROUTINES MODIFY THE BUFFER POINTER*/


int8_t get_rfInt8_from_packet(uint8_t **buffer_address)
{
    uint8_t *buffer = (uint8_t*)*buffer_address;
    int8_t data = (int8_t)buffer[0];
    *buffer_address += sizeof (int8_t);
    return data;
}


uint8_t get_rfUint8_from_packet(uint8_t **buffer_address)
{
    uint8_t *buffer = (uint8_t*)*buffer_address;
    uint8_t data = (uint8_t)buffer[0];
    *buffer_address += sizeof (uint8_t);
    return data;
}

char  get_rfChar_from_packet (uint8_t** buffer_address)
{
    uint8_t *buffer = (uint8_t*)*buffer_address;
    char data = (char)buffer[0];
    *buffer_address += sizeof (char);
    return data;
}

uint8_t  get_rfByte_from_packet (uint8_t** buffer_address)
{
    uint8_t *buffer = (uint8_t*)*buffer_address;
    uint8_t data = (uint8_t)buffer[0];
    *buffer_address += sizeof (uint8_t);
    return data;
}

uint8_t  get_rfBool_from_packet (uint8_t** buffer_address)
{
    uint8_t *buffer = (uint8_t*)*buffer_address;
    uint8_t data = (uint8_t)buffer[0];
    *buffer_address += sizeof (uint8_t);
    return data;
}



int16_t  get_rfInt16_from_packet (uint8_t** buffer_address, rf_endianess endian)
{
    uint8_t *buffer = (uint8_t*)*buffer_address;
    int16_t data = 0;
    if(endian == kEndianessLittle)
        data = ((int16_t)buffer[0]) | (((int16_t)buffer[1]) << 8);
    else
        data = ((int16_t)buffer[1]) | (((int16_t)buffer[0]) << 8);
    *buffer_address += sizeof (int16_t);
    return data;
}


uint16_t get_rfUint16_from_packet(uint8_t** buffer_address, rf_endianess endian)
{
    uint8_t *buffer = (uint8_t*)*buffer_address;
    uint16_t data = 0;
    if(endian == kEndianessLittle)
        data = ((uint16_t)buffer[0]) | (((uint16_t)buffer[1]) << 8);
    else
        data = ((uint16_t)buffer[1]) | (((uint16_t)buffer[0]) << 8);
    *buffer_address += sizeof (uint16_t);
    return data;
}



int32_t  get_rfInt32_from_packet (uint8_t** buffer_address, rf_endianess endian)
{
    uint8_t *buffer = (uint8_t*)*buffer_address;
    int32_t data = 0;
    if(endian == kEndianessLittle)
        data =  ((int32_t) buffer[0]) |
                (((int32_t)buffer[1]) << 8) |
                (((int32_t)buffer[2]) << 16) |
                (((int32_t)buffer[3]) << 24);
    else
        data =  ((int32_t) buffer[3]) |
                (((int32_t)buffer[2]) << 8) |
                (((int32_t)buffer[1]) << 16) |
                (((int32_t)buffer[0]) << 24);
    *buffer_address += sizeof (int32_t);
    return data;
}

uint32_t get_rfUint32_from_packet(uint8_t** buffer_address, rf_endianess endian)
{
    uint8_t *buffer = (uint8_t*)*buffer_address;
    uint32_t data = 0;
    if(endian == kEndianessLittle)
        data =  ((uint32_t) buffer[0]) |
                (((uint32_t)buffer[1]) << 8) |
                (((uint32_t)buffer[2]) << 16) |
                (((uint32_t)buffer[3]) << 24);
    else
        data =  ((uint32_t) buffer[3]) |
                (((uint32_t)buffer[2]) << 8) |
                (((uint32_t)buffer[1]) << 16) |
                (((uint32_t)buffer[0]) << 24);
    *buffer_address += sizeof (uint32_t);
    return data;
}

size_t get_array_from_packet(
        uint8_t* destination_buffer, uint8_t** source_buffer_address, size_t num)
{
    uint8_t *p = (uint8_t*)*source_buffer_address;
    memcpy(destination_buffer, p, num);
    return move_packet_n_bytes(num, source_buffer_address);
}



size_t add_rfInt8_to_packet (int8_t data, uint8_t** buffer_address)
{
     uint8_t *p = (uint8_t*)*buffer_address;
     p[0] = data;
     *buffer_address += 1;
     return 1;
}

size_t add_rfUint8_to_packet(uint8_t data, uint8_t** buffer_address)
{
    uint8_t *p = (uint8_t*)*buffer_address;
    p[0] = data;
    *buffer_address += 1;
    return 1;
}

size_t add_rfChar_to_packet (char data, uint8_t** buffer_address)
{
    uint8_t *p = (uint8_t*)*buffer_address;
    p[0] = data;
    *buffer_address += 1;
    return 1;
}

size_t add_rfByte_to_packet (uint8_t data, uint8_t** buffer_address)
{
    uint8_t *p = (uint8_t*)*buffer_address;
    p[0] = data;
    *buffer_address += 1;
    return 1;
}

size_t add_rfBool_to_packet (uint8_t data, uint8_t** buffer_address)
{
    uint8_t *p = (uint8_t*)*buffer_address;
    p[0] = data;
    *buffer_address += 1;
    return 1;
}

size_t add_rfInt16_to_packet (int16_t data, uint8_t** buffer_address, rf_endianess endian)
{
    uint8_t *p = (uint8_t*)*buffer_address;
    if(endian == kEndianessLittle)
    {
        p[0] = (data) & 0xFF;
        p[1] = (data >> 8) & 0xFF;
    }
    else
    {
        p[1] = (data) & 0xFF;
        p[0] = (data >> 8) & 0xFF;
    }

    (*buffer_address) += sizeof (int16_t);

    return sizeof (int16_t);
}

size_t add_rfUint16_to_packet(uint16_t data, uint8_t** buffer_address, rf_endianess endian)
{
    uint8_t *p = (uint8_t*)*buffer_address;
    if(endian == kEndianessLittle)
    {
        p[0] = (data) & 0xFF;
        p[1] = (data >> 8) & 0xFF;
    }
    else
    {
        p[1] = (data) & 0xFF;
        p[0] = (data >> 8) & 0xFF;
    }

    (*buffer_address) += sizeof (uint16_t);

    return sizeof (uint16_t);
}

size_t add_rfInt32_to_packet (int32_t data, uint8_t** buffer_address, rf_endianess endian)
{
    uint8_t *p = (uint8_t*)*buffer_address;
    if(endian == kEndianessLittle)
    {
        p[0] = (data) & 0xFF;
        p[1] = (data >> 8) & 0xFF;
        p[2] = (data >> 16) & 0xFF;
        p[3] = (data >> 24) & 0xFF;
    }
    else
    {
        p[3] = (data) & 0xFF;
        p[2] = (data >> 8) & 0xFF;
        p[1] = (data >> 16) & 0xFF;
        p[0] = (data >> 24) & 0xFF;
    }

    (*buffer_address) += sizeof (int32_t);

    return sizeof (int32_t);
}

size_t add_rfUint32_to_packet(uint32_t data, uint8_t** buffer_address, rf_endianess endian)
{
    uint8_t *p = (uint8_t*)*buffer_address;
    if(endian == kEndianessLittle)
    {
        p[0] = (data) & 0xFF;
        p[1] = (data >> 8) & 0xFF;
        p[2] = (data >> 16) & 0xFF;
        p[3] = (data >> 24) & 0xFF;
    }
    else
    {
        p[3] = (data) & 0xFF;
        p[2] = (data >> 8) & 0xFF;
        p[1] = (data >> 16) & 0xFF;
        p[0] = (data >> 24) & 0xFF;
    }

    (*buffer_address) += sizeof (uint32_t);

    return sizeof (uint32_t);
}

#ifdef _SUPPORT_64BIT_DATATYPES

uint64_t get_rfUint64_from_packet(uint8_t** buffer_address, rf_endianess endian)
{
    uint8_t *buffer = (uint8_t*)*buffer_address;
    uint64_t data = 0;
    if(endian == kEndianessLittle)
        data =  ((uint64_t) buffer[0]) |
                (((uint64_t)buffer[1]) << 8) |
                (((uint64_t)buffer[2]) << 16) |
                (((uint64_t)buffer[3]) << 24) |
                (((uint64_t)buffer[4]) << 32) |
                (((uint64_t)buffer[5]) << 40) |
                (((uint64_t)buffer[6]) << 48) |
                (((uint64_t)buffer[7]) << 56);
    else
        data =  ((uint64_t) buffer[7]) |
                (((uint64_t)buffer[6]) << 8) |
                (((uint64_t)buffer[5]) << 16) |
                (((uint64_t)buffer[4]) << 24) |
                (((uint64_t)buffer[3]) << 32) |
                (((uint64_t)buffer[2]) << 40) |
                (((uint64_t)buffer[1]) << 48) |
                (((uint64_t)buffer[0]) << 56);
    *buffer_address += sizeof (uint64_t);
    return data;
}

uint64_t get_rfInt64_from_packet(uint8_t** buffer_address, rf_endianess endian)
{
    uint8_t *buffer = (uint8_t*)*buffer_address;
    uint64_t data = 0;
    if(endian == kEndianessLittle)
        data =  ((int64_t) buffer[0]) |
                (((int64_t)buffer[1]) << 8) |
                (((int64_t)buffer[2]) << 16) |
                (((int64_t)buffer[3]) << 24) |
                (((int64_t)buffer[4]) << 32) |
                (((int64_t)buffer[5]) << 40) |
                (((int64_t)buffer[6]) << 48) |
                (((int64_t)buffer[7]) << 56);
    else
        data =  ((int64_t) buffer[7]) |
                (((int64_t)buffer[6]) << 8) |
                (((int64_t)buffer[5]) << 16) |
                (((int64_t)buffer[4]) << 24) |
                (((int64_t)buffer[3]) << 32) |
                (((int64_t)buffer[2]) << 40) |
                (((int64_t)buffer[1]) << 48) |
                (((int64_t)buffer[0]) << 56);
    *buffer_address += sizeof (int64_t);
    return data;
}

size_t add_rfUint64_to_packet(uint64_t data, uint8_t** buffer_address, rf_endianess endian)
{
    uint8_t *p = (uint8_t*)*buffer_address;
    if(endian == kEndianessLittle)
    {
        p[0] = (data) & 0xFF;
        p[1] = (data >> 8) & 0xFF;
        p[2] = (data >> 16) & 0xFF;
        p[3] = (data >> 24) & 0xFF;
        p[4] = (data >> 32) & 0xFF;
        p[5] = (data >> 40) & 0xFF;
        p[6] = (data >> 48) & 0xFF;
        p[7] = (data >> 56) & 0xFF;
    }
    else
    {
        p[7] = (data) & 0xFF;
        p[6] = (data >> 8) & 0xFF;
        p[5] = (data >> 16) & 0xFF;
        p[4] = (data >> 24) & 0xFF;
        p[3] = (data >> 24) & 0xFF;
        p[2] = (data >> 24) & 0xFF;
        p[1] = (data >> 24) & 0xFF;
        p[0] = (data >> 24) & 0xFF;
    }

    (*buffer_address) += sizeof (uint64_t);

    return sizeof (uint64_t);
}

size_t add_rfInt64_to_packet(int64_t data, uint8_t** buffer_address, rf_endianess endian)
{
    uint8_t *p = (uint8_t*)*buffer_address;
    if(endian == kEndianessLittle)
    {
        p[0] = (data) & 0xFF;
        p[1] = (data >> 8) & 0xFF;
        p[2] = (data >> 16) & 0xFF;
        p[3] = (data >> 24) & 0xFF;
        p[4] = (data >> 32) & 0xFF;
        p[5] = (data >> 40) & 0xFF;
        p[6] = (data >> 48) & 0xFF;
        p[7] = (data >> 56) & 0xFF;
    }
    else
    {
        p[7] = (data) & 0xFF;
        p[6] = (data >> 8) & 0xFF;
        p[5] = (data >> 16) & 0xFF;
        p[4] = (data >> 24) & 0xFF;
        p[3] = (data >> 24) & 0xFF;
        p[2] = (data >> 24) & 0xFF;
        p[1] = (data >> 24) & 0xFF;
        p[0] = (data >> 24) & 0xFF;
    }

    (*buffer_address) += sizeof (int64_t);

    return sizeof (int64_t);
}

#endif


void determine_endianess() {
    int i = 1;
    char *p = (char *)&i;
    if (p[0] == 1)
    {
        g_platform_endianess = kEndianessLittle;
    }
    else
    {
        g_platform_endianess = kEndianessBig;
    }
}

int8_t get_endianess(void) {
    return g_platform_endianess;
}

size_t move_packet_n_bytes(
        int n, uint8_t** packet_runner)
{
    (*packet_runner) += n;
    return n;
}

size_t fill_next_n_packet_bytes_with(
        uint8_t value, uint32_t n, uint8_t** packet)
{
    memset(*packet, value, n);
    return n;
}

size_t fill_next_n_packet_bytes_with_value_and_move_to_next_position(
        uint8_t value, uint32_t n, uint8_t** packet)
{
    fill_next_n_packet_bytes_with(value, n, packet);
    move_packet_n_bytes(n, (uint8_t**)packet);
    return n;
}

