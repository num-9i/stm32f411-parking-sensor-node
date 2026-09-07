# Pin Map

## MCU / Board

- MCU: STM32F411RET6
- Board: NUCLEO-F411RE
- System Clock: 84 MHz

## Peripheral Pin Assignment

| 기능 | Peripheral | Pin | 방향 | 설명 |
|---|---|---:|---|---|
| HC-SR04 Echo | TIM2_CH1 | PA0 | Input | Echo pulse Input Capture, Timer 분해능 1 us |
| HC-SR04 Trigger | GPIO | PA1 | Output | 초음파 측정 시작용 Trigger pulse 출력 |
| CLI TX | USART2_TX | PA2 | Output | ST-LINK Virtual COM Port |
| CLI RX | USART2_RX | PA3 | Input | ST-LINK Virtual COM Port |
| Onboard LED | GPIO | PA5 | Output | NUCLEO LD2, Bring-up / Debug 용도 |
| Modbus TX | USART1_TX | PA9 | Output | 자동 방향 제어 RS485 모듈의 RXD에 연결 |
| Modbus RX | USART1_RX | PA10 | Input | 자동 방향 제어 RS485 모듈의 TXD에 연결 |
| RGB Red | GPIO | PB12 | Output | OCCUPIED 상태 표시 |
| RGB Green | GPIO | PB13 | Output | FREE 상태 표시 |
| RGB Blue | GPIO | PB14 | Output | ERROR 상태 표시 |

## TIM2 Configuration

HC-SR04의 Echo pulse 폭을 측정하기 위해 `TIM2_CH1 Input Capture`를 사용합니다.

- Channel: CH1 Input Capture
- Timer Input Clock: 84 MHz
- Prescaler: 83
- Counter Frequency: 1 MHz
- Counter Resolution: 1 us
- Counter Period: 0xFFFFFFFF
- Initial Capture Polarity: Rising Edge
- Input Capture Prescaler: No Division
- Input Filter: 0
- TIM2 Global Interrupt: Enabled

Echo 신호의 Rising Edge에서 측정 시작 시점을 저장한 뒤, Capture Polarity를 Falling Edge로 변경해 종료 시점을 측정합니다.

```text
Rising Edge
    ↓
시작 시각 Capture
    ↓
Falling Edge
    ↓
종료 시각 Capture
    ↓
Echo Pulse Width 계산
    ↓
Distance 계산
```

측정한 Echo pulse width는 HC-SR04 드라이버에서 거리 값으로 변환합니다.

## UART Configuration

### USART2 — CLI / Debug

- Pins: PA2 / PA3
- Baud Rate: 115200
- Format: 8-N-1
- Hardware Flow Control: None
- Connection: ST-LINK Virtual COM Port

`USART2`는 CLI와 Debug 출력 전용으로 사용합니다.

Modbus 통신과 디버그 출력을 서로 다른 UART로 분리해, 외부 Modbus Master와 통신하는 동안에도 ST-LINK VCP를 통해 내부 상태를 확인할 수 있도록 구성했습니다.

### USART1 — Modbus RTU / RS485

- Pins: PA9 / PA10
- Baud Rate: 115200
- Format: 8-N-1
- Hardware Flow Control: None
- USART1 Global Interrupt: Enabled

현재 프로토타입의 RS485 연결 구성은 다음과 같습니다.

```text
NUCLEO PA9  (USART1_TX)
        ↓
Auto-direction RS485 Module RXD

NUCLEO PA10 (USART1_RX)
        ↑
Auto-direction RS485 Module TXD
```

사용 중인 RS485 모듈은 송수신 방향을 자동으로 전환합니다.


## HC-SR04 Electrical Interface

HC-SR04는 5 V 전원으로 동작합니다.

HC-SR04의 Echo 출력은 약 5 V이므로 STM32 입력 핀에 직접 연결하지 않고 저항 분압 회로를 사용했습니다.

현재 프로토타입의 분압 구성은 다음과 같습니다.

```text
HC-SR04 ECHO
    |
   1 kΩ
    |
    +------ PA0 / TIM2_CH1
    |
   2 kΩ
    |
   GND
```

이 구성으로 Echo 신호 전압을 약 3.3 V 수준으로 낮춘 뒤 STM32에 입력합니다.

## RGB LED

주차 상태를 표시하기 위해 RGB LED를 사용합니다.

RGB LED는 Active High 방식으로 제어합니다.

| Parking State | LED |
|---|---|
| FREE | Green |
| OCCUPIED | Red |
| ERROR | Blue |

사용 핀은 다음과 같습니다.

```text
PB12 → Red
PB13 → Green
PB14 → Blue
```

주차 상태 판정 결과에 따라 해당 색상의 LED를 출력합니다.

## RS485 Prototype Wiring

RS485 실통신 검증에는 자동 방향 제어 TTL-to-RS485 모듈과 USB-to-RS485 어댑터를 사용했습니다.

```text
STM32F411RE                  Auto-direction RS485 Module

PA9  / USART1_TX  -------->  RXD
PA10 / USART1_RX  <--------  TXD
GND                 -------  GND
5V                  -------  VCC

                             D+ / A ------ USB-RS485 A
                             D- / B ------ USB-RS485 B
                             GND    ------ USB-RS485 GND
```

RS485 모듈은 NUCLEO 보드의 5 V 전원을 사용합니다.

USB-to-RS485 어댑터의 `+5 V` 출력은 NUCLEO 보드에 연결하지 않고, `A`, `B`, `GND`만 연결했습니다.

## 현재 프로토타입 구조

현재 프로토타입의 주요 신호 흐름은 다음과 같습니다.

```text
HC-SR04
   │
   ├─ Trigger → PA1
   │
   └─ Echo → Voltage Divider → PA0 / TIM2_CH1
                                  │
                                  ↓
                         Distance Measurement
                                  │
                                  ↓
                         Parking State Logic
                         FREE / OCCUPIED / ERROR
                                  │
                    ┌─────────────┴─────────────┐
                    ↓                           ↓
                 RGB LED                   Modbus Registers
                                                │
                                                ↓
                                             USART1
                                                │
                                                ↓
                                             RS485
```

`USART2`는 위 제어 흐름과 별도로 CLI / Debug 용도로 사용합니다.

## PCB Design Note

현재 프로토타입에서는 자동 방향 제어 RS485 모듈을 사용하므로 별도의 `DE/RE` 제어 핀이 필요하지 않습니다.

이 Pin Map은 현재 프로토타입 기준이며, 최종 PCB 설계 단계에서 일부 핀 할당이 변경될 수 있습니다.