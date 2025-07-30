# 🌱 Vertical Farming Automation System

An intelligent microcontroller-based automation solution for vertical farming applications, featuring comprehensive sensor monitoring, automated irrigation control, climate management, and safety systems.

## 📋 Table of Contents
- [Overview](#overview)
- [Features](#features)
- [Hardware Components](#hardware-components)
- [System Architecture](#system-architecture)
- [Installation & Setup](#installation--setup)
- [Usage](#usage)
- [UART Commands](#uart-commands)
- [Sensor Details](#sensor-details)
- [Code Structure](#code-structure)
- [Troubleshooting](#troubleshooting)
- [Future Improvements](#future-improvements)
- [Contributing](#contributing)
- [License](#license)

## 🎯 Overview

This project implements a complete vertical farming automation system using the **PIC18F452** microcontroller. The system monitors environmental conditions, controls irrigation, manages lighting, and ensures safety through multiple sensors and actuators. All operations can be monitored and controlled via UART interface for remote management.

### Key Capabilities:
- **Multi-sensor monitoring**: Temperature, humidity, light levels, water temperature
- **Automated irrigation**: Water level sensing and pump control
- **Climate control**: Temperature and humidity monitoring with DHT11 and DS18B20
- **Safety systems**: Flame detection with alarm system
- **Lighting automation**: LDR-based automatic LED control
- **Remote control**: Complete UART command interface
- **Real-time logging**: Continuous system status monitoring

## ✨ Features

### 🌡️ Environmental Monitoring
- **DHT11 Sensor**: Air temperature and humidity measurement
- **DS18B20 Sensor**: Water temperature monitoring (±0.5°C accuracy)
- **LDR Sensor**: Ambient light level detection
- **Real-time data logging** with change detection

### 💧 Water Management
- **Ultrasonic sensor (HC-SR04)**: Non-contact water level measurement
- **Automated pump control**: Maintains water level between 60-70%
- **Bidirectional motor control**: Fill and drain operations
- **Water temperature monitoring**: Ensures optimal growing conditions

### 💡 Lighting System
- **Automatic LED control**: Based on ambient light levels
- **Multiple LED channels**: Independent control of 3 LED strips
- **Energy efficient**: Only activates when needed (LDR < 100)

### 🔥 Safety Systems
- **Flame detection**: Digital flame sensor with buzzer alarm
- **System status indicators**: Visual LED feedback for all operations
- **Emergency controls**: UART-based emergency shutdown

### 🔧 Control Interface
- **UART Communication**: 9600 baud rate for PC connectivity
- **Real-time commands**: Individual sensor/actuator control
- **Status monitoring**: Continuous system health reporting
- **Diagnostic mode**: Sensor functionality testing

## 🛠️ Hardware Components

### Microcontroller
- **PIC18F452**: 8-bit microcontroller (20MHz crystal)
- **Program memory**: 32KB Flash
- **Data memory**: 1536 bytes RAM

### Sensors
| Component | Pin | Function | Range |
|-----------|-----|----------|-------|
| DHT11 | Digital | Air temp/humidity | 0-50°C, 20-90% RH |
| DS18B20 | Digital | Water temperature | -55°C to +125°C |
| HC-SR04 | B1(Trig), B2(Echo) | Distance measurement | 2cm - 400cm |
| LDR | AN0 | Light intensity | 0-1023 ADC |
| Flame Sensor | E0 | Fire detection | Digital |

### Actuators
| Component | Pin | Function |
|-----------|-----|----------|
| Water Pump Motor | A4, A5 | Bidirectional control |
| Spraying Machine | C0, C1 | Irrigation system |
| LED Strips | B5, B6, B7 | Lighting control |
| Buzzer | D0 | Alarm system |

### Status LEDs
| LED | Pin | Indicates |
|-----|-----|-----------|
| DHT11_LED | D1 | Temperature sensor active |
| DS18B20_LED | D2 | Water temp sensor active |
| HCSR04_LED | D3 | Ultrasonic sensor active |
| WATER_PUMP_LED | D4 | Pump operation |
| SPRAYING_MACHINE_LED | D5 | Spraying system active |
| LDR_LED | D6 | Light sensor active |
| START_LED | E1 | System running |
| STOP_LED | E2 | System stopped |

## 🏗️ System Architecture

```
┌─────────────────┐    ┌──────────────────┐    ┌─────────────────┐
│   Sensors       │───▶│   PIC18F452      │───▶│   Actuators     │
│                 │    │   Controller     │    │                 │
│ • DHT11         │    │                  │    │ • Water Pump    │
│ • DS18B20       │    │ • ADC Module     │    │ • Spray System  │
│ • HC-SR04       │    │ • Timer Module   │    │ • LED Lights    │
│ • LDR           │    │ • UART Module    │    │ • Buzzer        │
│ • Flame Sensor  │    │ • GPIO Control   │    │                 │
└─────────────────┘    └──────────────────┘    └─────────────────┘
                              │
                              ▼
                    ┌──────────────────┐
                    │   PC/Terminal    │
                    │   (UART 9600)    │
                    └──────────────────┘
```

## 🚀 Installation & Setup

### Prerequisites
- **MPLAB X IDE** or compatible PIC compiler
- **CCS C Compiler** (code written in CCS C)
- **PIC programmer** (PIC Kit 3/4 or similar)
- **Serial terminal** software (PuTTY, Tera Term, etc.)

### Hardware Setup
1. **Power Supply**: Connect 5V regulated power to VDD
2. **Crystal Oscillator**: 20MHz crystal with 22pF capacitors
3. **UART Interface**: Connect pins C6(TX) and C7(RX) to serial adapter
4. **Sensor Connections**: Wire sensors according to pin assignments
5. **Pull-up Resistors**: Add 10kΩ pull-ups on digital inputs

### Software Installation
1. Clone this repository:
   ```bash
   git clone https://github.com/yourusername/vertical-farming-pic18f452.git
   ```
2. Open the project in MPLAB X IDE
3. Include required libraries:
   - `dht_11.c` - DHT11 sensor library
   - `ds18b20.c` - DS18B20 temperature sensor library
4. Compile and program the PIC18F452

### Circuit Diagram
*(Include your circuit schematic here - can be created with Fritzing, KiCad, or hand-drawn)*

## 📖 Usage

### System Startup
1. Power on the system
2. Connect UART terminal (9600 baud, 8N1)
3. System performs automatic sensor testing
4. Monitor startup messages for sensor status

### Normal Operation
The system operates autonomously with these behaviors:

- **Water Level Control**: 
  - Level < 60% → Pump fills tank (motor forward)
  - Level 60-70% → Pump stops
  - Level > 70% → Pump drains (motor reverse)

- **Lighting Control**:
  - Light level < 100 → LEDs turn on
  - Light level ≥ 100 → LEDs turn off

- **Temperature Monitoring**:
  - Continuous monitoring of air and water temperature
  - Data logged when values change

- **Safety Monitoring**:
  - Flame detection triggers immediate alarm
  - System status displayed via LEDs

## 🎮 UART Commands

Connect via serial terminal and use these commands:

### System Control
| Command | Function |
|---------|----------|
| `d` | Enable system |
| `f` | Disable system |

### Sensor Control
| Command | Function |
|---------|----------|
| `u` | Enable DHT11 sensor |
| `i` | Disable DHT11 sensor |
| `h` | Enable DS18B20 sensor |
| `g` | Disable DS18B20 sensor |
| `t` | Enable ultrasonic sensor |
| `y` | Disable ultrasonic sensor |
| `o` | Enable LDR sensor |
| `p` | Disable LDR sensor |

### Actuator Control
| Command | Function |
|---------|----------|
| `e` | Enable water pump |
| `r` | Disable water pump |
| `a` | Enable spraying machine |
| `s` | Disable spraying machine |

### Safety Control
| Command | Function |
|---------|----------|
| `q` | Trigger flame alarm |
| `w` | Reset flame alarm |

### Example Output
```
Vertical Farming SYS
Loading ...

LDR Sensor Working
Ultrasonic Sensor Working
DHT11 Sensor Working
DS18B20 Sensor Working
Sensor Test is Completed Successfully

[LOG] Status: 1 | Light: 0 | Temp: 24C | Humidity: 65% | Water Level: 45% | Spraying Machine: 0 | Motor Stat: 1 | Water Temp: 22.50
```

## 🔍 Sensor Details

### DHT11 Temperature & Humidity Sensor
- **Operating Range**: 0-50°C, 20-90% RH
- **Accuracy**: ±2°C, ±5% RH
- **Update Rate**: 1Hz (1 reading per second)
- **Interface**: Single-wire digital

### DS18B20 Water Temperature Sensor
- **Operating Range**: -55°C to +125°C
- **Accuracy**: ±0.5°C (from -10°C to +85°C)
- **Resolution**: 9 to 12-bit programmable
- **Interface**: 1-Wire digital

### HC-SR04 Ultrasonic Sensor
- **Operating Range**: 2cm to 400cm
- **Accuracy**: ±3mm
- **Measuring Angle**: 15 degrees
- **Trigger**: 10μs TTL pulse

### LDR (Light Dependent Resistor)
- **Resistance Range**: 200Ω (bright) to 10MΩ (dark)
- **ADC Resolution**: 10-bit (0-1023)
- **Response Time**: ~20ms

## 📁 Code Structure

```
├── main.c                 # Main program file
├── dht_11.c              # DHT11 sensor library
├── ds18b20.c             # DS18B20 sensor library
├── README.md             # This file
└── docs/
    ├── circuit_diagram.pdf
    └── user_manual.pdf
```

### Key Functions

#### Sensor Functions
- `dht11_control()` - Reads temperature and humidity
- `water_temp_function()` - Reads water temperature
- `distance_calc()` - Measures water level
- `led_check_system()` - Controls lighting based on LDR

#### Control Functions
- `motor_control()` - Manages water pump operation
- `spraying_machine_control()` - Controls irrigation system
- `flame_sensor_control()` - Handles fire safety

#### Communication
- `terminal_flame_alarm_control()` - UART command processing
- Interrupt-driven UART reception

## 🐛 Troubleshooting

### Common Issues

**Sensor Not Working**
- Check power connections (5V, GND)
- Verify pin assignments match code
- Test with multimeter for proper voltages

**UART Communication Issues**
- Verify baud rate (9600)
- Check TX/RX pin connections
- Ensure proper ground connection

**Water Pump Not Responding**
- Check motor driver connections
- Verify power supply capacity
- Test motor manually

**Inaccurate Readings**
- Allow sensors to stabilize (2-3 minutes)
- Check for electromagnetic interference
- Verify ADC reference voltage

### Diagnostic Commands
Use UART commands to test individual components:
1. Disable system with `f`
2. Test each sensor individually
3. Check status LEDs for feedback

## 🔮 Future Improvements

### Planned Features
- [ ] **WiFi Connectivity**: ESP8266 integration for IoT capabilities
- [ ] **Mobile App**: Android/iOS control interface
- [ ] **Data Logging**: SD card storage for historical data
- [ ] **pH Monitoring**: Water quality sensors
- [ ] **Camera Integration**: Plant growth monitoring
- [ ] **Machine Learning**: Predictive irrigation algorithms

### Hardware Upgrades
- [ ] Migration to STM32 for better performance
- [ ] LCD display for local status monitoring
- [ ] Relay modules for higher power devices
- [ ] Battery backup system

### Software Enhancements
- [ ] PID control for temperature regulation
- [ ] Configurable thresholds via UART
- [ ] Error logging and recovery systems
- [ ] Real-time clock for scheduling

## 🤝 Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

### Development Guidelines
1. Fork the repository
2. Create a feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit changes (`git commit -m 'Add AmazingFeature'`)
4. Push to branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

### Code Style
- Use clear, descriptive variable names
- Comment complex algorithms
- Follow CCS C compiler conventions
- Test thoroughly before submitting

## 📝 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 👨‍💻 Author

**Emir Kaan Demirdaş**
- 📧 Email: demirdasemirkaan@gmail.com
- 💼 LinkedIn: [emir-kaan-demirdas](https://www.linkedin.com/in/emir-kaan-demirdas/)

---

⭐ If this project helped you, please give it a star!

---

*Built with ❤️ for sustainable agriculture and IoT innovation*
