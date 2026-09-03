# Modbus Register Map

The parking sensor node operates as a Modbus RTU slave.

- Slave ID: `1`
- Baud rate: `115200`
- Data format: `8N1`
- Supported functions:
  - `0x03` Read Holding Registers
  - `0x06` Write Single Register

## Holding Registers

| Address | Name | Access | Default | Description |
|---|---|---|---:|---|
| `0x0000` | Distance_mm | R | - | Latest measured distance in millimeters |
| `0x0001` | Parking_State | R | - | Current parking state |
| `0x0002` | Status_Flags | R | - | Sensor and parking status bitfield |
| `0x0003` | Total_Error_Count | R | 0 | Sum of HC-SR04 error counters |
| `0x0004` | Rising_Timeout_Count | R | 0 | Rising-edge timeout count |
| `0x0005` | Falling_Timeout_Count | R | 0 | Falling-edge timeout count |
| `0x0006` | Invalid_Pulse_Count | R | 0 | Invalid echo pulse count |
| `0x0007` | Occupied_Threshold_mm | R/W | 500 | Distance threshold for OCCUPIED state |
| `0x0008` | Hysteresis_mm | R/W | 50 | Hysteresis added to occupied threshold |
| `0x0009` | Entry_Delay_ms | R/W | 500 | Required time below occupied threshold before entering OCCUPIED |
| `0x000A` | Exit_Delay_ms | R/W | 1000 | Required time above free threshold before entering FREE |

## Parking State

| Value | State |
|---:|---|
| `0` | FREE |
| `1` | OCCUPIED |
| `2` | ERROR |

## Status Flags

| Bit | Mask | Meaning |
|---:|---:|---|
| 0 | `0x0001` | Distance valid |
| 1 | `0x0002` | Sensor error |
| 2 | `0x0004` | Parking state is ERROR |

Example:

```text
Status_Flags = 0x0001
```

This means that the current distance measurement is valid and no sensor or parking error flag is active.

## Parking Threshold Logic

The FREE threshold is calculated at runtime as:

```text
Free_Threshold_mm =
    Occupied_Threshold_mm + Hysteresis_mm
```

With the default configuration:

```text
Occupied threshold = 500 mm
Hysteresis         = 50 mm
Free threshold     = 550 mm
Entry delay        = 500 ms
Exit delay         = 1000 ms
```

Therefore:

- `distance <= 500 mm` maintained for 500 ms → `OCCUPIED`
- `distance > 550 mm` maintained for 1000 ms → `FREE`
- `500 mm < distance <= 550 mm` → current parking state is retained

## Runtime Configuration

Registers `0x0007` through `0x000A` can be modified at runtime using Modbus function `0x06`.

Example:

```text
Register 0x0007

500 mm (0x01F4)
        ↓ FC06
300 mm (0x012C)
```

The updated value is applied immediately to the parking decision logic.

Configuration is currently stored in RAM only and returns to the default values after reset or power cycle.

## Validation

### Occupied Threshold

`Occupied_Threshold_mm` must be at least `20 mm`.

The configuration is rejected if:

```text
Occupied_Threshold_mm + Hysteresis_mm > 4000 mm
```

because the HC-SR04 measurement range used by the firmware is limited to 4000 mm.

### Hysteresis

The hysteresis value is rejected if:

```text
Occupied_Threshold_mm + Hysteresis_mm > 4000 mm
```

### Entry and Exit Delay

Entry and exit delays are stored as unsigned 16-bit millisecond values.

Valid range:

```text
0 to 65535 ms
```

A value of `0` effectively disables the corresponding transition delay.

## Error Counter Representation

The internal HC-SR04 error counters are stored as 32-bit values.

Because a Modbus holding register is 16 bits, values greater than `65535` are saturated to:

```text
0xFFFF
```

instead of wrapping around.

## Distance Validity

`Distance_mm` retains the latest stored distance value even when the current measurement becomes invalid.

Therefore, a Modbus client must check:

```text
Status_Flags bit 0
```

before treating `Distance_mm` as a valid current measurement.

## Modbus Exceptions

| Exception Code | Name | Usage |
|---:|---|---|
| `0x01` | Illegal Function | Unsupported Modbus function |
| `0x02` | Illegal Data Address | Invalid register address or write attempt to a read-only register |
| `0x03` | Illegal Data Value | Invalid quantity or rejected configuration value |

Examples verified over RS485:

```text
FC06 write to 0x0000
→ Exception 0x02

FC06 write 10 mm to 0x0007
→ Exception 0x03
```

## Byte Order

Modbus register values use big-endian byte order.

Example:

```text
500 decimal = 0x01F4

Register data:
01 F4
```

The Modbus CRC field is transmitted low byte first.

Example:

```text
CRC value = 0x0A84

Wire order:
84 0A
```

## Communication Flow

The current implementation uses USART1 for Modbus RTU communication.

```text
PC Modbus Client
    ↓
USB-RS485 Adapter
    ↓
RS485 Bus
    ↓
Auto-direction RS485 Module
    ↓
USART1
    ↓
Modbus RTU Parser
    ↓
Register Layer
    ↓
Parking Sensor Application
```

The RS485 module handles transmit/receive direction automatically, so no MCU-controlled DE/RE signal is required.

## Current Limitation

The UART receive path currently expects the supported `0x03` and `0x06` requests as fixed 8-byte Modbus RTU frames.

Generic variable-length Modbus RTU frame detection is not implemented.

This implementation is therefore intentionally scoped to the currently supported request formats rather than acting as a general-purpose Modbus RTU stack.