#ifndef RGB_LED_H
#define RGB_LED_H

#include "main.h"
#include <stdint.h>

typedef enum
{
    RGB_LED_OFF = 0,
    RGB_LED_RED,
    RGB_LED_GREEN,
    RGB_LED_BLUE
} RGB_LED_Color_t;

void RGB_LED_Init(void);
void RGB_LED_SetColor(RGB_LED_Color_t color);

#endif
