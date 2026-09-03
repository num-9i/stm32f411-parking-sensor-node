#include "modbus_rtu.h"

#include "modbus_crc.h"
#include "modbus_registers.h"

#include <stddef.h>


static uint16_t ModbusRTU_BuildException(
    uint8_t function,
    uint8_t exception_code,
    uint8_t *response,
    uint16_t response_capacity
)
{
    if (response_capacity < 5U)
    {
        return 0U;
    }

    response[0] = MODBUS_SLAVE_ID;
    response[1] = function | 0x80U;
    response[2] = exception_code;

    uint16_t crc = ModbusCRC16(response, 3U);

    response[3] = (uint8_t)(crc & 0x00FFU);
    response[4] = (uint8_t)((crc >> 8U) & 0x00FFU);

    return 5U;
}


uint16_t ModbusRTU_ProcessRequest(
    const uint8_t *request,
    uint16_t request_length,
    uint8_t *response,
    uint16_t response_capacity
)
{
    if ((request == NULL) || (response == NULL))
    {
        return 0U;
    }

    if (request_length != MODBUS_REQUEST_SIZE)
    {
        return 0U;
    }

    /* Ignore requests for another slave */
    if (request[0] != MODBUS_SLAVE_ID)
    {
        return 0U;
    }

    /* Check request CRC */
    uint16_t received_crc =
        (uint16_t)request[6]
        | ((uint16_t)request[7] << 8U);

    uint16_t calculated_crc =
        ModbusCRC16(request, 6U);

    if (received_crc != calculated_crc)
    {
        return 0U;
    }

    uint8_t function = request[1];

    uint16_t address =
        ((uint16_t)request[2] << 8U)
        | (uint16_t)request[3];

    uint16_t field =
        ((uint16_t)request[4] << 8U)
        | (uint16_t)request[5];


    /* ---------------------------------------------------------
     * Function 0x03 - Read Holding Registers
     * --------------------------------------------------------- */
    if (function == MODBUS_FUNC_READ_HOLDING)
    {
        uint16_t start_address = address;
        uint16_t quantity = field;

        if ((quantity == 0U) || (quantity > 125U))
        {
            return ModbusRTU_BuildException(
                function,
                MODBUS_EXCEPTION_ILLEGAL_VALUE,
                response,
                response_capacity
            );
        }

        if ((start_address >= MODBUS_REG_COUNT) ||
            (quantity > (MODBUS_REG_COUNT - start_address)))
        {
            return ModbusRTU_BuildException(
                function,
                MODBUS_EXCEPTION_ILLEGAL_ADDRESS,
                response,
                response_capacity
            );
        }

        uint16_t response_length =
            3U + (quantity * 2U) + 2U;

        if (response_length > response_capacity)
        {
            return 0U;
        }

        response[0] = MODBUS_SLAVE_ID;
        response[1] = MODBUS_FUNC_READ_HOLDING;
        response[2] = (uint8_t)(quantity * 2U);

        uint16_t tx_index = 3U;

        for (uint16_t i = 0U; i < quantity; i++)
        {
            uint16_t value = 0U;

            if (!ModbusRegisters_Read(
                    start_address + i,
                    &value))
            {
                return ModbusRTU_BuildException(
                    function,
                    MODBUS_EXCEPTION_ILLEGAL_ADDRESS,
                    response,
                    response_capacity
                );
            }

            /* Register data is big-endian */
            response[tx_index++] =
                (uint8_t)((value >> 8U) & 0x00FFU);

            response[tx_index++] =
                (uint8_t)(value & 0x00FFU);
        }

        uint16_t crc =
            ModbusCRC16(response, tx_index);

        /* CRC is little-endian on the wire */
        response[tx_index++] =
            (uint8_t)(crc & 0x00FFU);

        response[tx_index++] =
            (uint8_t)((crc >> 8U) & 0x00FFU);

        return tx_index;
    }


    /* ---------------------------------------------------------
     * Function 0x06 - Write Single Register
     * --------------------------------------------------------- */
    if (function == MODBUS_FUNC_WRITE_SINGLE)
    {
        uint16_t register_address = address;
        uint16_t register_value = field;

        ModbusRegisterWriteResult_t result =
            ModbusRegisters_Write(
                register_address,
                register_value
            );

        if (result == MODBUS_REG_WRITE_ILLEGAL_ADDRESS)
        {
            return ModbusRTU_BuildException(
                function,
                MODBUS_EXCEPTION_ILLEGAL_ADDRESS,
                response,
                response_capacity
            );
        }

        if (result == MODBUS_REG_WRITE_ILLEGAL_VALUE)
        {
            return ModbusRTU_BuildException(
                function,
                MODBUS_EXCEPTION_ILLEGAL_VALUE,
                response,
                response_capacity
            );
        }

        /*
         * Normal 0x06 response echoes the complete request.
         */
        if (response_capacity < MODBUS_REQUEST_SIZE)
        {
            return 0U;
        }

        for (uint16_t i = 0U; i < MODBUS_REQUEST_SIZE; i++)
        {
            response[i] = request[i];
        }

        return MODBUS_REQUEST_SIZE;
    }


    /* Unsupported function */
    return ModbusRTU_BuildException(
        function,
        MODBUS_EXCEPTION_ILLEGAL_FUNCTION,
        response,
        response_capacity
    );
}
