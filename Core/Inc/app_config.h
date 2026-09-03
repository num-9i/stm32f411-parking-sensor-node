#ifndef APP_CONFIG_H
#define APP_CONFIG_H

#include <stdint.h>

void AppConfig_Init(void);

uint16_t AppConfig_GetOccupiedThresholdMm(void);
uint16_t AppConfig_GetEntryDelayMs(void);
uint16_t AppConfig_GetExitDelayMs(void);

uint16_t AppConfig_GetHysteresisMm(void);


uint8_t AppConfig_SetOccupiedThresholdMm(uint16_t value);
uint8_t AppConfig_SetHysteresisMm(uint16_t value);
uint8_t AppConfig_SetEntryDelayMs(uint16_t value);
uint8_t AppConfig_SetExitDelayMs(uint16_t value);

#endif
