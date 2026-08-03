# System Architecture

## System Flow

```text
                         +------------------+
                         |     HC-SR04      |
                         +--------+---------+
                                  |
                                  v
                         +------------------+
                         |  hcsr04 driver   |
                         +--------+---------+
                                  |
                                  v
                         +------------------+
                         |  parking logic   |
                         +--------+---------+
                                  |
                                  v
                         +------------------+
                         |   parking data   |
                         +----+--------+----+
                              |        |
                 +------------+        +-------------+
                 |                                   |
                 v                                   v
        +------------------+               +------------------+
        |  RGB Status LED  |               |     UART CLI     |
        | FREE/OCC/ERROR   |               | USART2 / VCP     |
        +------------------+               +------------------+

                                  |
                                  v
                         +------------------+
                         |   Modbus RTU     |
                         | USART1 / RS485   |
                         +------------------+
```

## Module Responsibilities

### `hcsr04`

- Trigger pulse generation
- Rising and falling edge capture
- Pulse width calculation
- Distance conversion
- Timeout and error detection

### `parking_logic`

- `FREE / OCCUPIED / ERROR` determination
- Entry and exit delay
- Hysteresis
- Consecutive failure handling

### `parking_data`

- Latest distance
- Parking state
- Valid flags
- Status flags
- Error counters
- Shared data for CLI and Modbus

### `led_status`

- RGB LED output control
- Parking state indication
- Error state indication

### `uart_cmd`

- Runtime diagnostics
- Configuration commands
- Internal state inspection

### `modbus_rtu`

- CRC16 calculation
- Function `0x03`
- Function `0x06`
- Register access validation
- Exception responses

### `app_config`

- Installation height
- Detection threshold
- Entry and exit delay
- Configuration range validation