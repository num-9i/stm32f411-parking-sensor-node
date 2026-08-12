

#ifndef INC_PARKING_LOGIC_H_
#define INC_PARKING_LOGIC_H_

#include <stdint.h>

typedef enum
{
    PARKING_STATE_FREE = 0,
    PARKING_STATE_OCCUPIED,
    PARKING_STATE_ERROR
} ParkingState_t;

void ParkingLogic_Init(void);

void ParkingLogic_Update(
    uint32_t distance_mm,
    uint8_t distance_valid,
    uint32_t now_ms
);

ParkingState_t ParkingLogic_GetState(void);




#endif /* INC_PARKING_LOGIC_H_ */
