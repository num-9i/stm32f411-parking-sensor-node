#include "modbus_crc.h"

uint16_t ModbusCRC16(
    const uint8_t *data,
    uint16_t length
)
{
    uint16_t crc = 0xFFFFU;

    for (uint16_t i = 0U; i < length; i++)
    {
        crc ^= data[i];

        for (uint8_t bit = 0U; bit < 8U; bit++)
        {
            if (crc & 0x0001U)
            {
                crc >>= 1U;
                crc ^= 0xA001U;
            }
            else
            {
                crc >>= 1U;
            }
        }
    }

    return crc;
}
