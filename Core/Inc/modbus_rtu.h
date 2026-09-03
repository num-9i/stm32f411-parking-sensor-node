#ifndef MODBUS_RTU_H
#define MODBUS_RTU_H

#include <stdint.h>

#define MODBUS_SLAVE_ID              1U

#define MODBUS_FUNC_READ_HOLDING     0x03U

#define MODBUS_EXCEPTION_ILLEGAL_FUNCTION  0x01U
#define MODBUS_EXCEPTION_ILLEGAL_ADDRESS   0x02U
#define MODBUS_EXCEPTION_ILLEGAL_VALUE     0x03U

#define MODBUS_REQUEST_SIZE          8U
#define MODBUS_MAX_RESPONSE_SIZE     64U

#define MODBUS_FUNC_WRITE_SINGLE      0x06U
uint16_t ModbusRTU_ProcessRequest(
    const uint8_t *request,
    uint16_t request_length,
    uint8_t *response,
    uint16_t response_capacity
);

#endif
