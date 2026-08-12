
#include "parking_logic.h"

#define PARKING_OCCUPIED_THRESHOLD_MM   500U
#define PARKING_FREE_THRESHOLD_MM       550U

#define PARKING_ENTRY_DELAY_MS          500U
#define PARKING_EXIT_DELAY_MS          1000U
#define PARKING_ERROR_DELAY_MS   500U

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

    if (parking_state == PARKING_STATE_ERROR)
    {
        if (distance_mm <= PARKING_OCCUPIED_THRESHOLD_MM)
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


    if (parking_state == PARKING_STATE_FREE)
    {
        if (distance_mm <= PARKING_OCCUPIED_THRESHOLD_MM)
        {
            if (!transition_active)
            {
                transition_start_tick = now_ms;
                transition_active = 1U;
            }

            if ((now_ms - transition_start_tick) >=
                PARKING_ENTRY_DELAY_MS)
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


    else if (parking_state == PARKING_STATE_OCCUPIED)
    {
        if (distance_mm > PARKING_FREE_THRESHOLD_MM)
        {
            if (!transition_active)
            {
                transition_start_tick = now_ms;
                transition_active = 1U;
            }

            if ((now_ms - transition_start_tick) >=
                PARKING_EXIT_DELAY_MS)
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

