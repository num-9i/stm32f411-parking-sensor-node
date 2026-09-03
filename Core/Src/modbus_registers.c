#include "modbus_registers.h"

#include "hcsr04.h"
#include "parking_logic.h"
#include "app_config.h"

#include <limits.h>


static uint16_t ModbusRegisters_ClampU32(uint32_t value)
{
    if (value > UINT16_MAX)
    {
        return UINT16_MAX;
    }

    return (uint16_t)value;
}


uint8_t ModbusRegisters_Read(
    uint16_t address,
    uint16_t *value
)
{
    if (value == NULL)
    {
        return 0U;
    }

    switch (address)
    {
        case MODBUS_REG_DISTANCE_MM:
            *value =
                ModbusRegisters_ClampU32(
                    HCSR04_GetDistanceMm()
                );
            break;


        case MODBUS_REG_PARKING_STATE:
            *value =
                (uint16_t)ParkingLogic_GetState();
            break;


        case MODBUS_REG_STATUS_FLAGS:
        {
            uint16_t flags = 0U;

            if (HCSR04_IsDistanceValid())
            {
                flags |= MODBUS_FLAG_DISTANCE_VALID;
            }

            if (HCSR04_GetLastError() != HCSR04_ERROR_NONE)
            {
                flags |= MODBUS_FLAG_SENSOR_ERROR;
            }

            if (ParkingLogic_GetState() == PARKING_STATE_ERROR)
            {
                flags |= MODBUS_FLAG_PARKING_ERROR;
            }

            *value = flags;
            break;
        }


        case MODBUS_REG_TOTAL_ERROR_COUNT:
        {
            uint32_t total =
                HCSR04_GetRisingTimeoutCount()
                + HCSR04_GetFallingTimeoutCount()
                + HCSR04_GetInvalidPulseCount();

            *value = ModbusRegisters_ClampU32(total);
            break;
        }


        case MODBUS_REG_RISING_TIMEOUT:
            *value =
                ModbusRegisters_ClampU32(
                    HCSR04_GetRisingTimeoutCount()
                );
            break;


        case MODBUS_REG_FALLING_TIMEOUT:
            *value =
                ModbusRegisters_ClampU32(
                    HCSR04_GetFallingTimeoutCount()
                );
            break;


        case MODBUS_REG_INVALID_PULSE:
            *value =
                ModbusRegisters_ClampU32(
                    HCSR04_GetInvalidPulseCount()
                );
            break;


        case MODBUS_REG_OCC_THRESHOLD:
            *value =
                AppConfig_GetOccupiedThresholdMm();
            break;


        case MODBUS_REG_HYSTERESIS:
            *value =
                AppConfig_GetHysteresisMm();
            break;


        case MODBUS_REG_ENTRY_DELAY:
            *value =
                AppConfig_GetEntryDelayMs();
            break;


        case MODBUS_REG_EXIT_DELAY:
            *value =
                AppConfig_GetExitDelayMs();
            break;


        default:
            return 0U;
    }

    return 1U;
}

ModbusRegisterWriteResult_t ModbusRegisters_Write(
    uint16_t address,
    uint16_t value
)
{
    switch (address)
    {
        case MODBUS_REG_OCC_THRESHOLD:
            if (!AppConfig_SetOccupiedThresholdMm(value))
            {
                return MODBUS_REG_WRITE_ILLEGAL_VALUE;
            }
            break;

        case MODBUS_REG_HYSTERESIS:
            if (!AppConfig_SetHysteresisMm(value))
            {
                return MODBUS_REG_WRITE_ILLEGAL_VALUE;
            }
            break;

        case MODBUS_REG_ENTRY_DELAY:
            if (!AppConfig_SetEntryDelayMs(value))
            {
                return MODBUS_REG_WRITE_ILLEGAL_VALUE;
            }
            break;

        case MODBUS_REG_EXIT_DELAY:
            if (!AppConfig_SetExitDelayMs(value))
            {
                return MODBUS_REG_WRITE_ILLEGAL_VALUE;
            }
            break;

        default:
            return MODBUS_REG_WRITE_ILLEGAL_ADDRESS;
    }

    return MODBUS_REG_WRITE_OK;
}

