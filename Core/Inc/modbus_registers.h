#ifndef MODBUS_REGISTERS_H
#define MODBUS_REGISTERS_H

#include <stdint.h>

#define MODBUS_REG_DISTANCE_MM          0x0000U
#define MODBUS_REG_PARKING_STATE       0x0001U
#define MODBUS_REG_STATUS_FLAGS        0x0002U
#define MODBUS_REG_TOTAL_ERROR_COUNT   0x0003U
#define MODBUS_REG_RISING_TIMEOUT      0x0004U
#define MODBUS_REG_FALLING_TIMEOUT     0x0005U
#define MODBUS_REG_INVALID_PULSE       0x0006U
#define MODBUS_REG_OCC_THRESHOLD       0x0007U
#define MODBUS_REG_HYSTERESIS          0x0008U
#define MODBUS_REG_ENTRY_DELAY         0x0009U
#define MODBUS_REG_EXIT_DELAY          0x000AU

#define MODBUS_REG_COUNT               11U

#define MODBUS_FLAG_DISTANCE_VALID     (1U << 0)
#define MODBUS_FLAG_SENSOR_ERROR       (1U << 1)
#define MODBUS_FLAG_PARKING_ERROR      (1U << 2)

uint8_t ModbusRegisters_Read(
    uint16_t address,
    uint16_t *value
);


typedef enum
{
    MODBUS_REG_WRITE_OK = 0,
    MODBUS_REG_WRITE_ILLEGAL_ADDRESS,
    MODBUS_REG_WRITE_ILLEGAL_VALUE

} ModbusRegisterWriteResult_t;


ModbusRegisterWriteResult_t ModbusRegisters_Write(
    uint16_t address,
    uint16_t value
);


#endif
