
[![Arduino CI](https://github.com/RobTillaart/RS485/workflows/Arduino%20CI/badge.svg)](https://github.com/marketplace/actions/arduino_ci)
[![Arduino-lint](https://github.com/RobTillaart/RS485/actions/workflows/arduino-lint.yml/badge.svg)](https://github.com/RobTillaart/RS485/actions/workflows/arduino-lint.yml)
[![JSON check](https://github.com/RobTillaart/RS485/actions/workflows/jsoncheck.yml/badge.svg)](https://github.com/RobTillaart/RS485/actions/workflows/jsoncheck.yml)
[![License: MIT](https://img.shields.io/badge/license-MIT-green.svg)](https://github.com/RobTillaart/RS485/blob/master/LICENSE)
[![GitHub release](https://img.shields.io/github/release/RobTillaart/RS485.svg?maxAge=3600)](https://github.com/RobTillaart/RS485/releases)


# RS485

Arduino library for RS485.


## Description

RS485 is an **experimental** library to make half duplex communication easier.
The library implements the Stream interface so the user can use
**print()** and **write()** calls just like one does with **Serial**.

Preferably the library is to be used with a hardwareSerial as these 
can buffer incoming characters in the background.

The 0.2.0 version of the library has no (tested) protocol for multi-byte 
messages so the user must implement such on top of this class.


## Interface

#### Base

- **RS485(Stream stream, uint8_t sendPin, uint8_t deviceID = 0)** constructor.
The default device ID is 0 (typically master uses this, or if deviceID is not used).
The stream is typically Serial, and the baud rate, timeout etc. should be set 
via the Serial class. 
The sendPin is the pin that connects to the transmit/receive enable pins.
The library sets the pinMode and defaults it to LOW (receiving mode).
- **void     setMicrosPerByte(uint32_t baudRate)** set the delay per character needed.
This gives the hardware enough time to flush the buffer. 
- **uint32_t getMicrosPerByte()** returns the current delay in micros used.
- **void setTXmode()** explicitly set mode to transmitting / sending.
This implies that the device will stop listening on the RS485 bus.
- **void setRXmode()** explicitly set mode to receiving / listening.
This is the default behaviour of every RS485 device.
- **uint8_t getMode()** returns the current mode, 1 == TX, 0 == RX.


#### Stream interface

The most important commands of the Stream interface are:

- **int available()** returns the number of characters available in the buffer.
- **int read()** read a character from the RS485 stream.
- **int peek()** peeks a character ahead. (less used).
- **void flush()** flush the stream. (less used).
- https://www.arduino.cc/reference/en/language/functions/communication/stream/

All variations of **print()**, **println()** and **write()** can be used,
the library calculates the time needed to set the RS485 chip in transmit mode.

An important command from the stream interface is the **setTimeOut()** as
this allows reads on the RS485 bus that are limited


## Operation

A RS485 controlled device is typically waiting for a command or message 
and is therefore default in listening or receiving mode. Only when
the device needs to answer the library will automatically set the RS485 
chip in sending mode, wait for enough time to "flush the buffer" and 
resumes with listening.


#### Pull up resistors

Do not forget to use one pull up (A line) and one pull down (B line) 
at only one end of the bus.
Values depend on the length of the cables, start with 1 KΩ.


#### Wires

Preferred wire for RS485 is STP (Shielded Twisted Pair), however 
UTP (Unshielded) will works in many cases.
Typical for most applications CAT5 (100 Mbit) will do the job.

Note CAT5 has 4 x 2 twisted wires so there are 6 cables to spare.
These could be used e.g. to build a FULL DUPLEX version in which
every slave has 2 RS485 ports, one for receiving and one for sending.
Another application is to use these as power lines e.g 5 and 12 V.


#### yield()

For RTOS environments the **yield()** function needs to be called 
when code might be blocking. As the RS485 baud rate can be pretty low, 
the **write(array, length)** function can be blocking for too long 
so the function can call **yield()** every 4 milliseconds if enabled.

To enable **yield()** uncomment the following line in **RS485.cpp**

// #define RS485_YIELD_ENABLE  1

or use this flag in the compile line option

Note: the **yield()** calling version is substantial slower, depending 
on the baud rate. Use with care.

TODO: to be tested on ESP32 - RTOS .


## Future

- improve documentation
- setUsPerByte() parameter does not feel 100%
- add **send()** and **receive()** for longer messages.
  - which handshake?
  - dynamic buffer size?
  - should this be a sort of message class / struct. fixed size?
- add examples
- add unit tests


