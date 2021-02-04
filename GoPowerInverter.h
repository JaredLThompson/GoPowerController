#ifndef GoPowerInverter_h
#define GoPowerInverter_h
#include "Arduino.h"
#include <SomeSerial.h>

class GoPowerInverter {
  public:
  GoPowerInverter(HardwareSerial* hardSerial);
  GoPowerInverter(uint8_t rx, uint8_t tx);
  void begin(int speed);
  void powerOn();
  void powerOff();
  float getVBAT();
  
  struct returnValue {
    float RespString;
    int ReturnCode;
  };
  
  private:
  SomeSerial* mySerial;
};

#endif
