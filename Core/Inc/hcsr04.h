#ifndef HCSR04_H
#define HCSR04_H

#include "main.h"
#include <stdint.h>

typedef enum
{
    HCSR04_STATE_IDLE = 0,
    HCSR04_STATE_WAIT_RISING,
    HCSR04_STATE_WAIT_FALLING,
    HCSR04_STATE_DATA_READY,
    HCSR04_STATE_ERROR
} HCSR04_State_t;

typedef enum
{
    HCSR04_ERROR_NONE = 0,
    HCSR04_ERROR_RISING_TIMEOUT,
    HCSR04_ERROR_FALLING_TIMEOUT,
    HCSR04_ERROR_INVALID_PULSE
} HCSR04_Error_t;


void HCSR04_Init(TIM_HandleTypeDef *htim);

void HCSR04_StartMeasurement(void);

void HCSR04_ClearDataReady(void);

void HCSR04_InputCaptureCallback(TIM_HandleTypeDef *htim);



uint32_t HCSR04_GetPulseWidthUs(void);

uint8_t HCSR04_IsDataReady(void);

uint32_t HCSR04_GetDistanceMm(void);
uint8_t HCSR04_IsDistanceValid(void);

#endif
