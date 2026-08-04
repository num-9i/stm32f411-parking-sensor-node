# Pin Map

## MCU and Board

- MCU: STM32F411RET6
- Board: NUCLEO-F411RE
- System clock: 84 MHz

## Peripheral Pin Assignment

| Function | Peripheral | Pin | Direction | Description |
|---|---|---:|---|---|
| HC-SR04 Echo | TIM2_CH1 | PA0 | Input | 1 us resolution input capture |
| HC-SR04 Trigger | GPIO | PA1 | Output | Ultrasonic trigger pulse |
| CLI TX | USART2_TX | PA2 | Output | ST-LINK Virtual COM Port |
| CLI RX | USART2_RX | PA3 | Input | ST-LINK Virtual COM Port |
| Onboard LED | GPIO | PA5 | Output | NUCLEO LD2, bring-up test |
| Modbus TX | USART1_TX | PA9 | Output | RS485 transceiver DI |
| Modbus RX | USART1_RX | PA10 | Input | RS485 transceiver RO |
| RS485 DE/RE | GPIO | PB5 | Output | Transceiver direction control |
| RGB Red | GPIO | PB12 | Output | Parking state indicator |
| RGB Green | GPIO | PB13 | Output | Parking state indicator |
| RGB Blue | GPIO | PB14 | Output | Error/status indicator |

## Timer Configuration

### TIM2

- Channel: CH1 Input Capture
- Timer input clock: 84 MHz
- Prescaler: 83
- Counter frequency: 1 MHz
- Counter resolution: 1 us
- Counter period: 0xFFFFFFFF
- Initial capture polarity: Rising edge
- Input capture prescaler: No division
- Input filter: 0
- TIM2 global interrupt: Enabled

## UART Configuration

### USART2 — CLI and Debug

- Pins: PA2 / PA3
- Baud rate: 115200
- Format: 8-N-1
- Hardware flow control: None

### USART1 — Modbus RTU / RS485

- Pins: PA9 / PA10
- Baud rate: 115200
- Format: 8-N-1
- Hardware flow control: None

## Electrical Notes

- HC-SR04 Echo output is approximately 5 V.
- Echo must not be connected directly until the 3.3 V level interface is prepared.
- RS485 DE and /RE will be controlled together using PB5.
- RGB LED polarity will be finalized during schematic design.