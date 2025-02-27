//
//    FILE: SHT85.cpp
//  AUTHOR: Rob Tillaart
// VERSION: 0.3.2
//    DATE: 2021-02-10
// PURPOSE: Arduino library for the SHT85 temperature and humidity sensor
//          https://nl.rs-online.com/web/p/temperature-humidity-sensor-ics/1826530
//     URL: https://github.com/RobTillaart/SHT85
//

//  HISTORY:
//  0.1.0   2021-02-10  initial version
//  0.1.1   2021-03-13  initial release
//  0.1.2   2021-05-27  fix Arduino-lint
//  0.1.3   2021-08-06  expose raw data from sensor
//  0.1.4   2021-08-24  prevent heater to switch on too fast.
//                      update readme
//  0.2.0   2021-08-24  split off base class
//                      create derived classes SHT85, 30, 31, 35
//
//  0.3.0   2021-10-20  Fix #6 image in documentation, 
//                      update build-CI
//  0.3.1   2021-12-28  update library.json, readme, license, minor edits
//  0.3.2   2022-01-17  fix #8 add SHT_DEFAULT_ADDRESS + 2x begin()


#include "SHT85.h"


// SUPPORTED COMMANDS - single shot mode only
#define SHT_READ_STATUS       0xF32D
#define SHT_CLEAR_STATUS      0x3041

#define SHT_SOFT_RESET        0x30A2
#define SHT_HARD_RESET        0x0006

#define SHT_MEASUREMENT_FAST  0x2416    // page 10 datasheet
#define SHT_MEASUREMENT_SLOW  0x2400    // no clock stretching

#define SHT_HEAT_ON           0x306D
#define SHT_HEAT_OFF          0x3066
#define SHT_HEATER_TIMEOUT    180000UL  // milliseconds


SHT::SHT()
{
  _address        = 0;
  _lastRead       = 0;
  _rawTemperature = 0;
  _rawHumidity    = 0;
  _heatTimeout    = 0;
  _heaterStart    = 0;
  _heaterStop     = 0;
  _heaterOn       = false;
  _error          = SHT_OK;
  _type           = 0;
}


#if defined(ESP8266) || defined(ESP32)
bool SHT::begin(const uint8_t address, const uint8_t dataPin, const uint8_t clockPin)
{
  if ((address != 0x44) && (address != 0x45))
  {
    return false;
  }
  _address = address;

  _wire = &Wire;
  if ((dataPin < 255) && (clockPin < 255))
  {
    _wire->begin(dataPin, clockPin);
  } else {
    _wire->begin();
  }
  return reset();
}


bool SHT::begin(const uint8_t dataPin, const uint8_t clockPin)
{
  return begin(SHT_DEFAULT_ADDRESS, dataPin, clockPin);
}
#endif


bool SHT::begin(const uint8_t address,  TwoWire *wire)
{
  if ((address != 0x44) && (address != 0x45))
  {
    return false;
  }
  _address = address;
  _wire    = wire;
  _wire->begin();
  return reset();
}


bool SHT::begin(TwoWire *wire)
{
  return begin(SHT_DEFAULT_ADDRESS, wire);
}


bool SHT::read(bool fast)
{
  if (writeCmd(fast ? SHT_MEASUREMENT_FAST : SHT_MEASUREMENT_SLOW) == false)
  {
    return false;
  }
  delay(fast ? 4 : 15); // table 4 datasheet
  return readData(fast);
}


bool SHT::isConnected()
{
  _wire->beginTransmission(_address);
  int rv = _wire->endTransmission();
  if (rv != 0) _error = SHT_ERR_NOT_CONNECT;
  return (rv == 0);
}

#ifdef doc
// bit - description
// ==================
// 15 Alert pending status
//    '0': no pending alerts
//    '1': at least one pending alert - default
// 14 Reserved ‘0’
// 13 Heater status
//    '0’ : Heater OFF - default
//    '1’ : Heater ON
// 12 Reserved '0’
// 11 Humidity tracking alert
//    '0’ : no alert - default
//    '1’ : alert
// 10 Temp tracking alert
//    '0’ : no alert - default
//    '1’ : alert
// 9:5 Reserved '00000’
// 4 System reset detected
//    '0': no reset since last ‘clear status register’ command
//    '1': reset detected (hard or soft reset command or supply fail) - default
// 3:2 Reserved ‘00’
// 1 Command status
//    '0': last command executed successfully
//    '1': last command not processed. Invalid or failed checksum
// 0 Write data checksum status
//    '0': checksum of last write correct
//    '1': checksum of last write transfer failed
#endif


uint16_t SHT::readStatus()
{
  uint8_t status[3] = { 0, 0, 0 };
  // page 13 datasheet
  if (writeCmd(SHT_READ_STATUS) == false)
  {
    return 0xFFFF;
  }
  // 16 bit status + CRC
  if (readBytes(3, (uint8_t*) &status[0]) == false)
  {
    return 0xFFFF;
  }

  if (status[2] != crc8(status, 2)) 
  {
    _error = SHT_ERR_CRC_STATUS;
    return 0xFFFF;
  }

  return (uint16_t) (status[0] << 8) + status[1];
}


bool SHT::reset(bool hard)
{
  bool b = writeCmd(hard ? SHT_HARD_RESET : SHT_SOFT_RESET);
  if (b == false)
  {
    return false;
  }
  delay(1);  // table 4 datasheet
  return true;
}


void SHT::setHeatTimeout(uint8_t seconds)
{
  _heatTimeout = seconds;
  if (_heatTimeout > 180) _heatTimeout = 180;
}


bool SHT::heatOn()
{
  if (isHeaterOn()) return true;
  if ((_heaterStop > 0) && (millis() - _heaterStop < SHT_HEATER_TIMEOUT))
  {
    _error = SHT_ERR_HEATER_COOLDOWN;
    return false;
  }
  if (writeCmd(SHT_HEAT_ON) == false)
  {
    _error = SHT_ERR_HEATER_ON;
    return false;
  }
  _heaterStart = millis();
  _heaterOn    = true;
  return true;
}


bool SHT::heatOff()
{
  // always switch off the heater - ignore _heaterOn flag.
  if (writeCmd(SHT_HEAT_OFF) == false)
  {
    _error = SHT_ERR_HEATER_OFF;  // can be serious!
    return false;
  }
  _heaterStop = millis();
  _heaterOn   = false;
  return true;
}


bool SHT::isHeaterOn()
{
  if (_heaterOn == false)
  {
    return false;
  }
  // did not exceed time out
  if (millis() - _heaterStart < (_heatTimeout * 1000UL))
  {
    return true;
  }
  heatOff();
  return false;
}


bool SHT::requestData()
{
  if (writeCmd(SHT_MEASUREMENT_SLOW) == false)
  {
    return false;
  }
  _lastRequest = millis();
  return true;
}


bool SHT::dataReady()
{
  return ((millis() - _lastRequest) > 15);  // TODO MAGIC NR
}


bool SHT::readData(bool fast)
{
  uint8_t buffer[6];
  if (readBytes(6, (uint8_t*) &buffer[0]) == false)
  {
    return false;
  }

  if (!fast)
  {
    if (buffer[2] != crc8(buffer, 2)) 
    {
      _error = SHT_ERR_CRC_TEMP;
      return false;
    }
    if (buffer[5] != crc8(buffer + 3, 2)) 
    {
      _error = SHT_ERR_CRC_HUM;
      return false;
    }
  }

  _rawTemperature = (buffer[0] << 8) + buffer[1];
  _rawHumidity    = (buffer[3] << 8) + buffer[4];

  _lastRead = millis();

  return true;
}


int SHT::getError()
{
  int rv = _error;
  _error = SHT_OK;
  return rv;
}


//////////////////////////////////////////////////////////

uint8_t SHT::crc8(const uint8_t *data, uint8_t len) 
{
  // CRC-8 formula from page 14 of SHT spec pdf
  const uint8_t POLY(0x31);
  uint8_t crc(0xFF);

  for (uint8_t j = len; j; --j) 
  {
    crc ^= *data++;

    for (uint8_t i = 8; i; --i) 
    {
      crc = (crc & 0x80) ? (crc << 1) ^ POLY : (crc << 1);
    }
  }
  return crc;
}


bool SHT::writeCmd(uint16_t cmd)
{
  _wire->beginTransmission(_address);
  _wire->write(cmd >> 8 );
  _wire->write(cmd & 0xFF);
  if (_wire->endTransmission() != 0)
  {
    _error = SHT_ERR_WRITECMD;
    return false;
  }
  return true;
}


bool SHT::readBytes(uint8_t n, uint8_t *val)
{
  int rv = _wire->requestFrom(_address, (uint8_t) n);
  if (rv == n)
  {
    for (uint8_t i = 0; i < n; i++)
    {
      val[i] = _wire->read();
    }
    return true;
  }
  _error = SHT_ERR_READBYTES;
  return false;
}



////////////////////////////////////////////////////////
//
// DERIVED
//
SHT30::SHT30()
{
  _type = 30;
};


SHT31::SHT31()
{
  _type = 31;
};


SHT35::SHT35()
{
  _type = 35;
};


SHT85::SHT85()
{
  _type = 85;
};


// -- END OF FILE --
