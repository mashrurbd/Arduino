#pragma once
//
//    FILE: AD520X.h
//  AUTHOR: Rob Tillaart
//    DATE: 2020-07-24
// VERSION: 0.2.2
// PURPOSE: Arduino library for AD5204 and AD5206 digital potentiometers (+ older AD8400, AD8402, AD8403)
//     URL: https://github.com/RobTillaart/AD520X
//
// HISTORY:
// see AD520X.cpp file
//


#include "Arduino.h"
#include "SPI.h"


#define AD520X_LIB_VERSION              (F("0.2.2"))


#ifndef AD520X_MIDDLE_VALUE
#define AD520X_MIDDLE_VALUE             128
#endif


class AD520X
{
public:
  AD520X(uint8_t select, uint8_t reset, uint8_t shutdown, uint8_t dataOut, uint8_t clock);  

  void     begin(uint8_t value = AD520X_MIDDLE_VALUE);


  //  MONO / SINGLE
  bool     setValue(uint8_t pm = 0, uint8_t value = AD520X_MIDDLE_VALUE);
  //  STEREO / DOUBLE
  bool     setValue(uint8_t pmA, uint8_t pmB, uint8_t value);
  void     setAll(uint8_t value = AD520X_MIDDLE_VALUE);
  uint8_t  getValue(uint8_t pm = 0);

  //  MONO / SINGLE
  bool     setPercentage(uint8_t pm = 0, float percentage = 50);
  //  STEREO / DOUBLE
  bool     setPercentage(uint8_t pmA, uint8_t pmB, float percentage);
  float    getPercentage(uint8_t pm = 0);


  void     reset(uint8_t value = AD520X_MIDDLE_VALUE);
  uint8_t  pmCount()   { return _pmCount; };


  void     powerOn()   { digitalWrite(_shutdown, LOW); };
  void     powerOff()  { digitalWrite(_shutdown, HIGH); };
  void     powerDown() { powerOff(); };             // obsolete, remove in 0.3.0
  bool     isPowerOn() { return digitalRead(_shutdown) == LOW; };


  //       speed in Hz
  void     setSPIspeed(uint32_t speed);
  uint32_t getSPIspeed() { return _SPIspeed; };

  // debugging
  bool     usesHWSPI() { return _hwSPI; };

  // ESP32 specific
  #if defined(ESP32)
  void     selectHSPI() { _useHSPI = true;  };
  void     selectVSPI() { _useHSPI = false; };
  bool     usesHSPI()   { return _useHSPI;  };
  bool     usesVSPI()   { return !_useHSPI; };

  // to overrule ESP32 default hardware pins
  void     setGPIOpins(uint8_t clk, uint8_t miso, uint8_t mosi, uint8_t select);
  #endif


protected:
  uint8_t  _dataOut;
  uint8_t  _clock;
  uint8_t  _select;
  uint8_t  _reset;
  uint8_t  _shutdown;
  bool     _hwSPI = 3;
  uint32_t _SPIspeed = 16000000;

  uint8_t  _value[6];
  uint8_t  _pmCount = 6;

  void     updateDevice(uint8_t pm, uint8_t value);
  void     swSPI_transfer(uint8_t value);

  SPIClass    * mySPI;
  SPISettings _spi_settings;

  #if defined(ESP32)
  bool        _useHSPI = true;
  #endif
};


/////////////////////////////////////////////////////////////////////////////
//
// DERIVED CLASSES
//
class AD5206 : public AD520X
{
public:
  AD5206(uint8_t select, uint8_t reset, uint8_t shutdown, uint8_t dataOut = 255, uint8_t clock = 255);
};


class AD5204 : public AD520X
{
public:
  AD5204(uint8_t select, uint8_t reset, uint8_t shutdown, uint8_t dataOut = 255, uint8_t clock = 255); 
};


class AD8400 : public AD520X
{
public:
  AD8400(uint8_t select, uint8_t reset, uint8_t shutdown, uint8_t dataOut = 255, uint8_t clock = 255); 
};


class AD8402 : public AD520X
{
public:
  AD8402(uint8_t select, uint8_t reset, uint8_t shutdown, uint8_t dataOut = 255, uint8_t clock = 255); 
};


class AD8403 : public AD520X
{
public:
  AD8403(uint8_t select, uint8_t reset, uint8_t shutdown, uint8_t dataOut = 255, uint8_t clock = 255); 
};

// -- END OF FILE -- 
