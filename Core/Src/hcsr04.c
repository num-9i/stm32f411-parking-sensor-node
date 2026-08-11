#include "hcsr04.h"


static TIM_HandleTypeDef *hcsr04_htim = NULL;

static volatile HCSR04_State_t hcsr04_state = HCSR04_STATE_IDLE;

static volatile uint32_t rising_capture = 0;

static volatile uint32_t falling_capture = 0;

static volatile uint32_t pulse_width_us = 0;

static volatile uint8_t data_ready = 0;


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

    __HAL_TIM_SET_CAPTUREPOLARITY(
        hcsr04_htim,
        TIM_CHANNEL_1,
        TIM_INPUTCHANNELPOLARITY_RISING
    );

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

        __HAL_TIM_SET_CAPTUREPOLARITY(
            htim,
            TIM_CHANNEL_1,
            TIM_INPUTCHANNELPOLARITY_RISING
        );

        data_ready = 1;
        hcsr04_state = HCSR04_STATE_DATA_READY;
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

