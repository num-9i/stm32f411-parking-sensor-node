#include "hcsr04.h"

#define HCSR04_RISING_TIMEOUT_MS   5U
#define HCSR04_FALLING_TIMEOUT_MS 30U

#define HCSR04_MIN_DISTANCE_MM    20U
#define HCSR04_MAX_DISTANCE_MM  4000U


static TIM_HandleTypeDef *hcsr04_htim = NULL;

static volatile HCSR04_State_t hcsr04_state = HCSR04_STATE_IDLE;

static volatile uint32_t rising_capture = 0;

static volatile uint32_t falling_capture = 0;

static volatile uint32_t pulse_width_us = 0;

static volatile uint8_t data_ready = 0;

static volatile uint32_t distance_mm = 0;

static volatile uint8_t distance_valid = 0;



static volatile HCSR04_Error_t last_error = HCSR04_ERROR_NONE;

static volatile uint32_t rising_timeout_count = 0;
static volatile uint32_t falling_timeout_count = 0;
static volatile uint32_t invalid_pulse_count = 0;

static uint32_t measurement_start_tick = 0;
static uint32_t rising_tick = 0;


static uint32_t HCSR04_PulseToDistanceMm(uint32_t pulse_us)
{
	return (pulse_us *343U) / 2000U;

}



void HCSR04_ClearDataReady(void)
{
    data_ready = 0;
    hcsr04_state = HCSR04_STATE_IDLE;
}

void HCSR04_Init(TIM_HandleTypeDef *htim)
{
    hcsr04_htim = htim;

    hcsr04_state = HCSR04_STATE_IDLE;

    rising_capture = 0;
    falling_capture = 0;
    pulse_width_us = 0;

    data_ready = 0;

    distance_mm = 0;
    distance_valid = 0;

    __HAL_TIM_SET_CAPTUREPOLARITY(

        hcsr04_htim,
        TIM_CHANNEL_1,
        TIM_INPUTCHANNELPOLARITY_RISING
    );

    HAL_TIM_IC_Start_IT(hcsr04_htim, TIM_CHANNEL_1);
}

void HCSR04_StartMeasurement(void)
{
    if (hcsr04_htim == NULL)
    {
        return;
    }

    if (hcsr04_state != HCSR04_STATE_IDLE)
    {
        return;
    }

    data_ready = 0;
    distance_valid = 0U;
    last_error = HCSR04_ERROR_NONE;


    __HAL_TIM_SET_CAPTUREPOLARITY(
        hcsr04_htim,
        TIM_CHANNEL_1,
        TIM_INPUTCHANNELPOLARITY_RISING
    );

    measurement_start_tick = HAL_GetTick();

    hcsr04_state = HCSR04_STATE_WAIT_RISING;

    HAL_GPIO_WritePin(
        HCSR04_TRIG_GPIO_Port,
        HCSR04_TRIG_Pin,
        GPIO_PIN_SET
    );

    uint32_t start = __HAL_TIM_GET_COUNTER(hcsr04_htim);

    while ((__HAL_TIM_GET_COUNTER(hcsr04_htim) - start) < 10U)
    {
        /* bounded 10 us wait */
    }

    HAL_GPIO_WritePin(
        HCSR04_TRIG_GPIO_Port,
        HCSR04_TRIG_Pin,
        GPIO_PIN_RESET
    );
}

void HCSR04_InputCaptureCallback(TIM_HandleTypeDef *htim)
{
    if (htim != hcsr04_htim)
    {
        return;
    }

    if (htim->Channel != HAL_TIM_ACTIVE_CHANNEL_1)
    {
        return;
    }

    if (hcsr04_state == HCSR04_STATE_WAIT_RISING)
    {
        rising_capture =
            HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);

        rising_tick = HAL_GetTick();

        __HAL_TIM_SET_CAPTUREPOLARITY(
            htim,
            TIM_CHANNEL_1,
            TIM_INPUTCHANNELPOLARITY_FALLING
        );

        hcsr04_state = HCSR04_STATE_WAIT_FALLING;
    }

    else if (hcsr04_state == HCSR04_STATE_WAIT_FALLING)
    {
        falling_capture =
            HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);

        pulse_width_us = falling_capture - rising_capture;

        distance_mm = HCSR04_PulseToDistanceMm(pulse_width_us);

        if((distance_mm >= HCSR04_MIN_DISTANCE_MM) && (distance_mm <= HCSR04_MAX_DISTANCE_MM))
        {
        	distance_valid = 1U;
        	last_error = HCSR04_ERROR_NONE;

        }
        else
        {
        	distance_valid = 0U;
        	last_error = HCSR04_ERROR_INVALID_PULSE;
        	invalid_pulse_count++;

        }

        __HAL_TIM_SET_CAPTUREPOLARITY(
            htim,
            TIM_CHANNEL_1,
            TIM_INPUTCHANNELPOLARITY_RISING
        );

        data_ready = 1;
        hcsr04_state = HCSR04_STATE_DATA_READY;
    }
}

void HCSR04_Task(void)
{
    uint32_t now = HAL_GetTick();

    if (hcsr04_state == HCSR04_STATE_WAIT_RISING)
    {
        if ((now - measurement_start_tick) >= HCSR04_RISING_TIMEOUT_MS)
        {
            distance_valid = 0U;
            last_error = HCSR04_ERROR_RISING_TIMEOUT;
            rising_timeout_count++;

            data_ready = 1U;
            hcsr04_state = HCSR04_STATE_DATA_READY;
        }
    }

    else if (hcsr04_state == HCSR04_STATE_WAIT_FALLING)
    {
        if ((now - rising_tick) >= HCSR04_FALLING_TIMEOUT_MS)
        {
            distance_valid = 0U;
            last_error = HCSR04_ERROR_FALLING_TIMEOUT;
            falling_timeout_count++;

            __HAL_TIM_SET_CAPTUREPOLARITY(
                hcsr04_htim,
                TIM_CHANNEL_1,
                TIM_INPUTCHANNELPOLARITY_RISING
            );

            data_ready = 1U;
            hcsr04_state = HCSR04_STATE_DATA_READY;
        }
    }
}


uint32_t HCSR04_GetPulseWidthUs(void)
{
    return pulse_width_us;
}

uint8_t HCSR04_IsDataReady(void)
{
    return data_ready;
}

uint32_t HCSR04_GetDistanceMm(void)
{
    return distance_mm;
}

uint8_t HCSR04_IsDistanceValid(void)
{
    return distance_valid;
}


HCSR04_Error_t HCSR04_GetLastError(void)
{
    return last_error;
}

uint32_t HCSR04_GetRisingTimeoutCount(void)
{
    return rising_timeout_count;
}

uint32_t HCSR04_GetFallingTimeoutCount(void)
{
    return falling_timeout_count;
}

uint32_t HCSR04_GetInvalidPulseCount(void)
{
    return invalid_pulse_count;
}

