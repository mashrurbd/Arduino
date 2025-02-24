
[![Arduino CI](https://github.com/RobTillaart/INA219/workflows/Arduino%20CI/badge.svg)](https://github.com/marketplace/actions/arduino_ci)
[![Arduino-lint](https://github.com/RobTillaart/INA219/actions/workflows/arduino-lint.yml/badge.svg)](https://github.com/RobTillaart/INA219/actions/workflows/arduino-lint.yml)
[![JSON check](https://github.com/RobTillaart/INA219/actions/workflows/jsoncheck.yml/badge.svg)](https://github.com/RobTillaart/INA219/actions/workflows/jsoncheck.yml)
[![License: MIT](https://img.shields.io/badge/license-MIT-green.svg)](https://github.com/RobTillaart/INA219/blob/master/LICENSE)
[![GitHub release](https://img.shields.io/github/release/RobTillaart/INA219.svg?maxAge=3600)](https://github.com/RobTillaart/INA219/releases)


# INA219

Arduino library for INA219 voltage, current and power sensor.


## Description

**Experimental** library for the INA219 power sensor.

Not tested as I have no hardware available.
So usage remarks and comments are welcome.

**USE WITH CARE**

The INA219 is a voltage, current and power measurement device. 
Maxima, see datasheet, chapter 7, esp 7.5

|  description  |  max  |  unit  | notes |
|:--------------|------:|-------:|:------|
| bus voltage   |  32   | Volt   | depends on BRNG setting
| shunt voltage |  320  | mVolt  | depends on PGA setting

#### special chars

- Ω == Ohm = ALT-234 (Windows)
- µ == micro = ALT-0181 (Windows)


## I2C

#### Address

The sensor can be configured to use 1 of 16 I2C addresses between 0x40 and 0x4F.
The address depends on how the A0 and A1 address lines are connected to the SCL, SDA, GND and VCC pins.
(datasheet chapter 8.5.5.1 Serial Bus Address)


#### Performance

Datasheet states it supports 1 KHz .. 2.56 MHz. (to be verified).
Note: higher speeds need smaller pull up resistors.


## Interface

Read datasheet for details.


### Constructor

- **INA219(const uint8_t address, TwoWire \*wire = Wire)** Constructor to set address and optional Wire interface.
- **bool begin(const uint8_t sda, const uint8_t scl)** for ESP32 and ESP8266; initializes the class.
sets I2C pins. 
Returns true if the INA219 address is on the I2C bus.
- **bool begin()** UNO ea. initializes the class. 
Returns true if the INA219 address is on the I2C bus.
- **bool isConnected()** returns true if the INA219 address is on the I2C bus.


### Core Functions

Note the power and the current are not meaningful without calibrating
the sensor. Also the value is not meaningful if there is no shunt connected.

- **float getShuntVoltage()** idem.
- **float getBusVoltage()** idem. Max 32 Volt.
- **float getPower()** is the current x BusVoltage in Watt.
- **float getCurrent()** is the current through the shunt in Ampere.

Helper functions to convert to the right scale of units.

- **float getBusVoltage_mV()** idem, in millivolts.
- **float getShuntVoltage_mV()** idem, in millivolts.
- **float getCurrent_mA()** idem in milliAmpere.
- **float getPower_mW()** idem in milliWatt.
- **float getShuntVoltage_uV()** idem microVolt.
- **float getCurrent_uA()** idem in microAmpere.
- **float getPower_uW()** idem, in microWatt.


### Configuration

Note: the conversion time runs in the background and if done value is stored in a register. 
The core functions read from the registers, so they are not blocked, 
but just get the same value if no new is ready.

- **void reset()** software power on reset. 
This implies calibration with **setMaxCurrentShunt()** needs to be redone.
- **bool setBusVoltageRange(uint8_t voltage = 16)** set to 16 or 32.
Values < 16 map to 16 an values between 16 and 32 map to 32.
- **uint8_t getBusVoltageRange()** returns 16 or 32.
- **bool setGain(uint8_t factor =,  1)** factor = 1, 2, 4, 8.
Determines the shunt voltage range. 40, 80, 160 or 320 mV. 
- **uint8_t getGain()** returns set factor.
- **bool setBusADC(uint8_t mask = 0x03)** check datasheet for meaning of mask.
Returns false if mask > 0x0F.
- **uint8_t getBusADC()** returns mask.
- **bool setShuntADC(uint8_t mask = 0x03)** check datasheet for meaning of mask.
Returns false if mask > 0x0F.
- **uint8_t getShuntADC()** returns mask.

mask = both resolution + averaging multiple samples.
minus - == don't care

|  bit mask  |  value  |  description      | conversion time |
|:----------:|:-------:|:------------------|:---------------:|
|    0-00    |  0 / 4  |  9 bit resolution |      84 μs      |
|    0-01    |  1 / 5  | 10 bit resolution |     148 μs      |
|    0-10    |  2 / 6  | 11 bit resolution |     276 μs      |
|    0-11    |  3 / 7  | 12 bit resolution |     532 μs      |
|    1000    |    8    | 12 bit 1 sample   |     532 μs      |
|    1001    |    9    |        2 samples  |    1.06 ms      |
|    1010    |   10    |        4 samples  |    2.13 ms      |
|    1011    |   11    |        8 samples  |    4.26 ms      |
|    1100    |   12    |       16 samples  |    8.51 ms      |
|    1101    |   13    |       32 samples  |   17.02 ms      |
|    1110    |   14    |       64 samples  |   34.05 ms      |
|    1111    |   15    |      128 samples  |   68.10 ms      |

- note that a new value can take a while depending on value set.
- note that you cannot set e.g. 9 bits and 16 samples.


### Operating mode

See datasheet,

- **bool setMode(uint8_t mode = 7)** mode = 0 .. 7
- **uint8_t getMode()** returns the mode (0..7) set.

Descriptive mode functions (wrappers).

- **bool shutDown()** mode 0
- **bool setModeShuntTrigger()** mode 1 - how to trigger to be investigated
- **bool setModeBusTrigger()** mode 2 
- **bool setModeShuntBusTrigger()** mode 3
- **bool setModeADCOff()** mode 4 - 
- **bool setModeShuntContinuous()** mode 5
- **bool setModeBusContinuous()** mode 6
- **bool setModeShuntBusContinuous()** mode 7 - default


### Calibration

See datasheet,

Calibration is mandatory for **getCurrent()** and **getPower()** to work.

- **bool setMaxCurrentShunt(float ampere = 20.0, float ohm = 0.002)** 
set the calibration register based upon the shunt and the max ampere. 
From this the LSB is derived. 
Note the function will round up the LSB to nearest round value by default. 
This may cause loss of precision. The function may force normalization if underflow detected.
The user **must** check the return value == true, otherwise the calibration register is **not** set.
- **bool isCalibrated()** returns true if CurrentLSB has been calculated by **setMaxCurrentShunt()**. 
- **float getCurrentLSB()** returns the LSB in Ampere == precision of the calibration.
- **float getCurrentLSB_mA()** returns the LSB in milliampere.
- **float getCurrentLSB_uA()** returns the LSB in microampere.
- **float getShunt()** returns the value set for the shunt.
- **float getMaxCurrent()** returns the value for the maxCurrent which can be corrected.

To print these values one might use https://github.com/RobTillaart/printHelpers 
to get scientific notation like "3.5e-6"


### debugging

- **uint16_t getRegister(uint8_t reg)** fetch registers directly, meant for debugging only.


## Operational

See examples.. 


## Future

#### Must

- get hardware (+ time) to test
- test different loads
- write and verify examples
  - all functions.
- update documentation

#### Should 

- create unit tests
- performance
  - verify I2C bus speed
  - verify conversion time
- Conversion ready flag  (8.6.3.2)
- Math overflow flag  (8.6.3.2)
- add a **setCurrentLSB(uint16_t mA)** function ?
  - maxAmpere as derived value

#### Could

- calibration
  - initial current 20A and shunt 0.002 Ω in **begin()** ??
  - autocorrect \_current_LSB  round number
  - maxCurrent? how much?
  - can the calibration math be optimized
    - integer only?
    - less iterations?
    - local var for current_lsb?
    - ??
- disconnected load, 
  - can it be recognized? => current drop?
- cache configuration ? ==> 2 bytes
  - what is gained? both getting and setting is faster.
    a few milliseconds per call?
  - about a dozen times used,
  - flag for forced read in functions **setMode(uint8_t mode, bool forced = false)**
- separate release notes.



