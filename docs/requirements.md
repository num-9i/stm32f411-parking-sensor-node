# Project Requirements

## Goal

STM32F411RE 기반으로 초음파 거리 측정, 주차 상태 판정,
RGB LED 표시, UART CLI 및 Modbus RTU/RS485 통신이 가능한
주차 상태 센서 노드를 구현한다.

## Parking States

- FREE
- OCCUPIED
- ERROR

## Required Features

- HC-SR04 Trigger pulse generation
- TIM Input Capture based Echo measurement
- Rising edge timeout
- Falling edge timeout
- Invalid pulse detection
- Distance measurement in millimeters
- Entry and exit delay
- Hysteresis
- RGB LED status indication
- UART CLI
- Modbus RTU Function 0x03
- Modbus RTU Function 0x06
- Cause-specific error counters
- KiCad Carrier PCB
- Board Bring-up and waveform verification

## Out of Scope

- Database
- Web or mobile application
- Multi-floor parking map
- Modbus TCP display system
- ESP32 wireless communication