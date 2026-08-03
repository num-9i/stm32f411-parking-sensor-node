# STM32F411RE Parking Sensor Node

## Overview

STM32F411RE 기반으로 HC-SR04 거리 측정, 주차 상태 판정,
RGB LED 표시 및 Modbus RTU/RS485 통신을 구현하는 센서 노드 프로젝트입니다.

현재 프로젝트는 요구사항 설정 및 초기 설계 단계입니다.

## Planned Features

- TIM Input Capture 기반 HC-SR04 Echo pulse 측정
- Non-blocking sensor measurement
- FREE / OCCUPIED / ERROR 상태 판정
- Entry/exit delay 및 hysteresis
- UART CLI
- Modbus RTU Function 0x03 / 0x06
- RS485 communication
- KiCad Carrier PCB
- PCB Bring-up 및 Logic Analyzer 검증

## Development Status

- [x] Repository initialized
- [ ] Pin map definition
- [ ] HC-SR04 driver
- [ ] Parking state logic
- [ ] UART CLI
- [ ] Modbus RTU
- [ ] Schematic and PCB
- [ ] Board Bring-up
