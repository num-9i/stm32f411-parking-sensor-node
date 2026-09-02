#include "rgb_led.h"

static void RGB_LED_AllOff(void)
{
    HAL_GPIO_WritePin(RGB_R_GPIO_Port, RGB_R_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(RGB_G_GPIO_Port, RGB_G_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(RGB_B_GPIO_Port, RGB_B_Pin, GPIO_PIN_RESET);
}

void RGB_LED_Init(void)
{
    RGB_LED_AllOff();
}

void RGB_LED_SetColor(RGB_LED_Color_t color)
{
    RGB_LED_AllOff();

    switch (color)
    {
        case RGB_LED_RED:
            HAL_GPIO_WritePin(
                RGB_R_GPIO_Port,
                RGB_R_Pin,
                GPIO_PIN_SET
            );
            break;

        case RGB_LED_GREEN:
            HAL_GPIO_WritePin(
                RGB_G_GPIO_Port,
                RGB_G_Pin,
                GPIO_PIN_SET
            );
            break;

        case RGB_LED_BLUE:
            HAL_GPIO_WritePin(
                RGB_B_GPIO_Port,
                RGB_B_Pin,
                GPIO_PIN_SET
            );
            break;

        case RGB_LED_OFF:
        default:
            break;
    }
}
