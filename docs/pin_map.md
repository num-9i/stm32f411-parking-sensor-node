# Pin Map

## MCU / Board

- MCU: STM32F411RET6
- Board: NUCLEO-F411RE
- System Clock: 84 MHz

## Peripheral Pin Assignment

| 기능 | Peripheral | Pin | 방향 | 설명 |
|---|---|---:|---|---|
| HC-SR04 Echo | TIM2_CH1 | PA0 | Input | Echo pulse Input Capture, Timer 분해능: 1 us |
| HC-SR04 Trigger | GPIO | PA1 | Output | 초음파 측정 시작용 Trigger pulse 출력 |
| Debug / CLI TX | USART2_TX | PA2 | Output | ST-LINK Virtual COM Port |
| Debug / CLI RX | USART2_RX | PA3 | Input | ST-LINK Virtual COM Port, 향후 CLI 입력용 |
| Onboard LED | GPIO | PA5 | Output | NUCLEO LD2, Bring-up / Debug 용도 |
| Modbus TX | USART1_TX | PA9 | Output | RS485 Transceiver DI / Prototype Module RXD |
| Modbus RX | USART1_RX | PA10 | Input | RS485 Transceiver RO / Prototype Module TXD |
| RS485 DE/RE | GPIO | PB5 | Output | 최종 Carrier PCB MAX3485 방향 제어, LOW=RX / HIGH=TX |
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

### USART2 — Debug / Future CLI

- Pins: PA2 / PA3
- Baud Rate: 115200
- Format: 8-N-1
- Hardware Flow Control: None
- Connection: ST-LINK Virtual COM Port

현재 `USART2`는 센서 측정값과 주차 상태 확인을 위한 Debug 출력에 사용합니다.

향후 UART CLI를 추가할 경우 동일한 USART2 / ST-LINK VCP를 사용할 수 있도록 핀을 할당했습니다.

Modbus 통신과 Debug UART를 서로 분리해, 외부 Modbus Master와 통신하는 동안에도 ST-LINK VCP를 통해 내부 상태를 확인할 수 있도록 구성했습니다.

### USART1 — Modbus RTU / RS485

- Pins: PA9 / PA10
- Baud Rate: 115200
- Format: 8-N-1
- Hardware Flow Control: None
- USART1 Global Interrupt: Enabled

현재 브레드보드 프로토타입의 RS485 연결 구성은 다음과 같습니다.

```text
NUCLEO PA9 (USART1_TX)
        ↓
Auto-direction RS485 Module RXD

NUCLEO PA10 (USART1_RX)
        ↑
Auto-direction RS485 Module TXD
```

프로토타입에서 사용하는 RS485 모듈은 송수신 방향을 자동으로 전환합니다.

최종 Carrier PCB에서는 `MAX3485`를 사용하며, `/RE`와 `DE`를 하나의 제어 신호로 묶어 `PB5`에서 직접 제어합니다.

```text
PB5 LOW
    ↓
DE  = 0
/RE = 0
    ↓
Receive Mode

PB5 HIGH
    ↓
DE  = 1
/RE = 1
    ↓
Transmit Mode
```

Modbus 응답 송신 직전에 `PB5`를 HIGH로 설정하고, `HAL_UART_Transmit()` 완료 후 LOW로 복귀시켜 다시 수신 상태로 전환합니다.

현재 브레드보드 프로토타입의 자동 방향 제어 RS485 모듈에는 `PB5`가 물리적으로 연결되지 않으므로, 이 GPIO 제어는 프로토타입 통신에는 영향을 주지 않습니다.

## HC-SR04 Electrical Interface

HC-SR04는 5 V 전원으로 동작합니다.

HC-SR04의 Echo 출력은 약 5 V이므로 STM32 입력 핀에 직접 연결하지 않고 저항 분압 회로를 사용했습니다.

현재 프로토타입과 Carrier PCB의 분압 구성은 다음과 같습니다.

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

PA9  / USART1_TX  --------> RXD
PA10 / USART1_RX  <-------- TXD
GND                 ------- GND
5V                  ------- VCC

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

`USART2`는 위 제어 흐름과 별도로 Debug 출력에 사용합니다.

## PCB Design Note

브레드보드 프로토타입과 최종 Carrier PCB의 RS485 방향 제어 방식은 다릅니다.

| 구분 | RS485 Interface | Direction Control |
|---|---|---|
| Prototype | Auto-direction TTL-to-RS485 Module | 모듈 내부 자동 전환 |
| Carrier PCB | MAX3485 | PB5 GPIO로 `/RE` + `DE` 직접 제어 |

최종 Carrier PCB에서는 `PB5`를 `RS485_DE_RE` 신호로 사용합니다.

PCB에는 `PB5` 신호에 10 kΩ Pull-down 저항을 적용해 MCU Reset 중에도 RS485 Driver가 비활성화되고 Receiver가 활성화되도록 구성했습니다.

Carrier PCB의 RS485 신호 연결은 다음과 같습니다.

```text
PA9 / USART1_TX  → MAX3485 DI
PA10 / USART1_RX ← MAX3485 RO
PB5              → MAX3485 DE + /RE

MAX3485 A        → RS485 A
MAX3485 B        → RS485 B
```

A-B 사이의 120 Ω 종단저항은 Solder Jumper를 통해 선택적으로 연결할 수 있도록 구성했습니다.