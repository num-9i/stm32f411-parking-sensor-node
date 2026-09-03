#include "app_config.h"

#define DEFAULT_OCCUPIED_THRESHOLD_MM  500U
#define DEFAULT_ENTRY_DELAY_MS         500U
#define DEFAULT_EXIT_DELAY_MS         1000U
#define DEFAULT_HYSTERESIS_MM 50U

static uint16_t hysteresis_mm;

static uint16_t occupied_threshold_mm;
static uint16_t entry_delay_ms;
static uint16_t exit_delay_ms;

void AppConfig_Init(void)
{
	hysteresis_mm = DEFAULT_HYSTERESIS_MM;
    occupied_threshold_mm = DEFAULT_OCCUPIED_THRESHOLD_MM;
    entry_delay_ms = DEFAULT_ENTRY_DELAY_MS;
    exit_delay_ms = DEFAULT_EXIT_DELAY_MS;
}

uint16_t AppConfig_GetOccupiedThresholdMm(void)
{
    return occupied_threshold_mm;
}

uint16_t AppConfig_GetEntryDelayMs(void)
{
    return entry_delay_ms;
}

uint16_t AppConfig_GetExitDelayMs(void)
{
    return exit_delay_ms;
}

uint16_t AppConfig_GetHysteresisMm(void)
{
    return hysteresis_mm;
}



uint8_t AppConfig_SetOccupiedThresholdMm(uint16_t value)
{
    if (value < 20U)
    {
        return 0U;
    }

    if (((uint32_t)value + hysteresis_mm) > 4000U)
    {
        return 0U;
    }

    occupied_threshold_mm = value;

    return 1U;
}


uint8_t AppConfig_SetHysteresisMm(uint16_t value)
{
    if (((uint32_t)occupied_threshold_mm + value) > 4000U)
    {
        return 0U;
    }

    hysteresis_mm = value;

    return 1U;
}


uint8_t AppConfig_SetEntryDelayMs(uint16_t value)
{
    entry_delay_ms = value;

    return 1U;
}


uint8_t AppConfig_SetExitDelayMs(uint16_t value)
{
    exit_delay_ms = value;

    return 1U;
}

