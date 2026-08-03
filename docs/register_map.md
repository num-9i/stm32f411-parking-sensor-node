# Initial Modbus Register Map

| Address | Name | Access | Description |
|---:|---|:---:|---|
| 0x0000 | Distance_mm | R | Latest valid distance |
| 0x0001 | Parking_State | R | FREE / OCCUPIED / ERROR |
| 0x0002 | Status_Flags | R | Valid and operating flags |
| 0x0003 | Total_Error_Count | R | Total sensor errors |
| 0x0004 | Rising_Timeout_Count | R | Rising edge timeout |
| 0x0005 | Falling_Timeout_Count | R | Falling edge timeout |
| 0x0006 | Invalid_Pulse_Count | R | Invalid Echo pulse |
| 0x0007 | Installation_Height_mm | R/W | Sensor installation height |
| 0x0008 | Detection_Threshold_mm | R/W | Occupancy threshold |
| 0x0009 | Entry_Delay_ms | R/W | Entry confirmation time |
| 0x000A | Exit_Delay_ms | R/W | Exit confirmation time |