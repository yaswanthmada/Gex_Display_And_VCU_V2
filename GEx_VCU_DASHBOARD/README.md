# GEX_VCU_Dashboard

## Project Overview

**GEX_VCU_Dashboard** is an embedded firmware project developed using the **STM32F103CBT6** microcontroller and a **DWIN HMI Display**. The Vehicle Control Unit (VCU) acts as the central controller responsible for collecting, processing, and displaying vehicle information.

The VCU receives real-time data over the **CAN bus** from multiple vehicle subsystems, including:

- Battery Management System (BMS)
- Motor Controller
- MPPT (Maximum Power Point Tracking) Controller

In addition to CAN communication, the VCU monitors the vehicle's digital and analog I/O signals to support various control and monitoring functions.

After processing the received data and executing the required vehicle logic, the VCU transmits the processed information to the **DWIN HMI Display** through the **UART** interface, providing the driver with real-time vehicle status, warnings, and operational information.

The firmware is designed with a modular architecture, making it easier to maintain, extend, and integrate additional vehicle features in the future.


## Features
- CAN communication with multiple vehicle subsystems
- Battery Management System (BMS) interface
- Motor Controller interface
- MPPT controller interface
- UART communication with DWIN HMI Display
- Digital I/O monitoring
- Analog I/O monitoring
- Real-time dashboard updates
- Vehicle status monitoring
- Fault and warning indication
- Communication timeout detection
- Vehicle control logic processing
- Modular firmware architecture
- Easy integration of additional vehicle subsystems
- Scalable software design
## System Architecture
```
                     +----------------------+
                     |  Battery Management  |
                     |      System (BMS)    |
                     +----------+-----------+
                                 |
                               CAN
                                 |
                                 |
+----------------------+         |
|  Motor Controller    |---------+
+----------------------+         |
                                 |
                                 |
+----------------------+         |
|        MPPT          |---------+
+----------------------+         |
                                 |
                                 v
               +----------------------------------+
               |       STM32F103CBT6              |
               |   Vehicle Control Unit (VCU)     |
               +---------+---------------+--------+
                         |               |
                    UART |               | GPIO / ADC
                         |               |
                         |               +-------------------+
                         |                                   |
                         v                                   v
            +----------------------+      +---------------------------+
            |   DWIN HMI Display   |      | Digital & Analog Inputs   |
            +----------------------+      | (Switches, Sensors, etc.) |
                                          +---------------------------+
```
## Repository Structure
```
GEX_VCU_Dashboard/
│
├── STM32/                 # STM32 firmware source code
├── DWIN/                  # DWIN HMI project and display resources
├── Documentation/         # Project documentation
├── datasheets/            # Component and hardware datasheets
├── images/                # Architecture diagrams and screenshots
├── README.md              # Project overview
└── .gitignore             # Git ignore rules
```

## Getting Started

### Prerequisites

- STM32CubeIDE
- ST-Link Programmer
- DWIN DGUS Tool
- Git

### Setup

1. Clone the repository.
2. Open the STM32 project in STM32CubeIDE.
3. Open the DWIN project using the DGUS Tool (if display changes are required).
4. Build the firmware.
5. Flash the firmware to the STM32F103CBT6 using ST-Link.
6. Power on the system and verify communication with the DWIN display and CAN devices.

## Documentation

- Project Overview
- System Architecture
- Hardware Overview
- Software Architecture
- CAN Communication
- BMS Interface
- Motor Controller Interface
- MPPT Interface
- Dashboard Interface
- IO Mapping
- State Machine
- Fault Management
- Build Guide
- Debugging
- Testing

## Future Development

- CAN Bootloader for firmware updates
- Firmware update over CAN without ST-Link
