#pragma once
//
//    FILE: DHT20.h
//  AUTHOR: Rob Tillaart
// PURPOSE: Arduino library for DHT20 I2C temperature and humidity sensor.
// VERSION: 0.3.1
//     URL: https://github.com/RobTillaart/DHT20
//

//  Always check datasheet - front view
//
//          +--------------+
//  VDD ----| 1            |
//  SDA ----| 2    DHT20   |
//  GND ----| 3            |
//  SCL ----| 4            |
//          +--------------+


#include "Arduino.h"
#include "Wire.h"

#ifdef ENABLE_SOFTWARE_I2C
#include "SoftWire.h"
#endif

#define DHT20_LIB_VERSION                    (F("0.3.1"))

#define DHT20_OK                             0
#define DHT20_ERROR_CHECKSUM                -10
#define DHT20_ERROR_CONNECT                 -11
#define DHT20_MISSING_BYTES                 -12
#define DHT20_ERROR_BYTES_ALL_ZERO          -13
#define DHT20_ERROR_READ_TIMEOUT            -14
#define DHT20_ERROR_LASTREAD                -15

class I2C_Interface {

    virtual void begin() = 0;
    virtual void beginTransmission(uint8_t address) = 0;
    virtual uint8_t endTransmission() = 0;
    virtual void write(uint8_t address, uint8_t data) = 0;
    virtual int read(uint8_t address, uint8_t *data, uint8_t length) = 0;
    virtual uint8_t requestFrom(uint8_t address, uint8_t length) = 0;
};

class HardwareI2C : public I2C_Interface {
  private:
    TwoWire* _wire;
  public:

    HardwareI2C(TwoWire *wire) {
      _wire = wire;
    }

    void begin() {
      _wire->begin();
    };

    void beginTransmission(uint8_t address) override {
      return _wire.beginTransmission(address);
    };

    uint8_t endTransmission() override {
      return _wire.endTransmission();
    };

    void write(uint8_t address, uint8_t data) override {
      return _wire.write(data);
    };
    int read(uint8_t address, uint8_t *data, uint8_t length) override {
      return _wire.readBytes(data, length);
    };

    uint8_t requestFrom(uint8_t address, uint8_t length) override {
      return _wire.requestFrom(address, length);
    };
};

#ifdef ENABLE_SOFTWARE_I2C
class SoftwareI2C : public I2C_Interface {
  private:
    SoftWire* _wire;

  public:
    SoftwareI2C(uint8_t sda, uint8_t scl) {
      _wire = new SoftWire(sda, scl)
    }

    void begin() {
      return _wire->begin();
    };

    void beginTransmission(uint8_t address) override {
      return _wire.beginTransmission(address);
    };

    uint8_t endTransmission() override {
      return _wire.endTransmission();
    };

    void write(uint8_t address, uint8_t data) override {
      return _wire.write(data);
    };

    int read(uint8_t address, uint8_t *data, uint8_t length) override {
      return _wire.readBytes(data, length);
    };

    uint8_t requestFrom(uint8_t address, uint8_t length) override {
      return _wire.requestFrom(address, length);
    };

}
#endif

class DHT20
{
public:
  //  CONSTRUCTOR
  //  fixed address 0x38
  DHT20(TwoWire *wire = &Wire);
#ifdef ENABLE_SOFTWARE_I2C
  DHT20(uint8_t sda, uint8_t scl);
#endif


  bool     begin();
  bool     isConnected();
  uint8_t  getAddress();


  //  ASYNCHRONUOUS CALL
  //  trigger acquisition.
  int      requestData();
  //  read the raw data.
  int      readData();
  //  converts raw data bits to temperature and humidity.
  int      convert();


  //  SYNCHRONOUS CALL
  //  blocking read call to read + convert data
  int      read();
  //  access the converted temperature & humidity
  float    getHumidity();
  float    getTemperature();


  //  OFFSET  1st order adjustments
  void     setHumOffset(float offset = 0);
  void     setTempOffset(float offset = 0);
  float    getHumOffset();
  float    getTempOffset();


  //  READ STATUS
  uint8_t  readStatus();
  //  3 wrapper functions around readStatus()
  bool     isCalibrated();
  bool     isMeasuring();
  bool     isIdle();
  //  status from last read()
  int      internalStatus();


  //  TIMING
  uint32_t lastRead();
  uint32_t lastRequest();


  //  RESET  (new since 0.1.4)
  //  use with care
  //  returns number of registers reset => must be 3
  //  3     = OK
  //  0,1,2 = error.
  //  255   = no reset needed.
  //  See datasheet 7.4 Sensor Reading Process, point 1
  //  use with care
  uint8_t  resetSensor();


private:
  float    _humidity;
  float    _temperature;
  float    _humOffset;
  float    _tempOffset;

  uint8_t  _status;
  uint32_t _lastRequest;
  uint32_t _lastRead;
  uint8_t  _bits[7];

  uint8_t  _crc8(uint8_t *ptr, uint8_t len);

  //  use with care
  bool     _resetRegister(uint8_t reg);

  //TwoWire* _wire;
  I2C_Interface* _wire;
};


//  -- END OF FILE --

