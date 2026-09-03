#include "parking_logic.h"
#include "app_config.h"


#define PARKING_ERROR_DELAY_MS  500U

static ParkingState_t parking_state = PARKING_STATE_ERROR;

static uint32_t transition_start_tick = 0U;
static uint8_t transition_active = 0U;

static uint32_t error_start_tick = 0U;
static uint8_t error_active = 0U;


void ParkingLogic_Init(void)
{
    parking_state = PARKING_STATE_ERROR;

    transition_start_tick = 0U;
    transition_active = 0U;

    error_start_tick = 0U;
    error_active = 0U;
}


void ParkingLogic_Update(
    uint32_t distance_mm,
    uint8_t distance_valid,
    uint32_t now_ms
)
{
    uint32_t occupied_threshold_mm =
        AppConfig_GetOccupiedThresholdMm();



    uint32_t entry_delay_ms =
        AppConfig_GetEntryDelayMs();

    uint32_t exit_delay_ms =
        AppConfig_GetExitDelayMs();

    uint32_t free_threshold_mm =
        occupied_threshold_mm + AppConfig_GetHysteresisMm();


    /* Sensor error handling */
    if (!distance_valid)
    {
        transition_active = 0U;

        if (!error_active)
        {
            error_start_tick = now_ms;
            error_active = 1U;
        }

        if ((now_ms - error_start_tick) >= PARKING_ERROR_DELAY_MS)
        {
            parking_state = PARKING_STATE_ERROR;
        }

        return;
    }

    error_active = 0U;


    /* Recover from ERROR on valid measurement */
    if (parking_state == PARKING_STATE_ERROR)
    {
        if (distance_mm <= occupied_threshold_mm)
        {
            parking_state = PARKING_STATE_OCCUPIED;
        }
        else
        {
            parking_state = PARKING_STATE_FREE;
        }

        transition_active = 0U;
        return;
    }


    /* FREE -> OCCUPIED */
    if (parking_state == PARKING_STATE_FREE)
    {
        if (distance_mm <= occupied_threshold_mm)
        {
            if (!transition_active)
            {
                transition_start_tick = now_ms;
                transition_active = 1U;
            }

            if ((now_ms - transition_start_tick) >= entry_delay_ms)
            {
                parking_state = PARKING_STATE_OCCUPIED;
                transition_active = 0U;
            }
        }
        else
        {
            transition_active = 0U;
        }
    }


    /* OCCUPIED -> FREE */
    else if (parking_state == PARKING_STATE_OCCUPIED)
    {
        if (distance_mm > free_threshold_mm)
        {
            if (!transition_active)
            {
                transition_start_tick = now_ms;
                transition_active = 1U;
            }

            if ((now_ms - transition_start_tick) >= exit_delay_ms)
            {
                parking_state = PARKING_STATE_FREE;
                transition_active = 0U;
            }
        }
        else
        {
            transition_active = 0U;
        }
    }
}


ParkingState_t ParkingLogic_GetState(void)
{
    return parking_state;
}
