#include "GoPowerInverter.h"

GoPowerInverter::GoPowerInverter(HardwareSerial* hardSerial) {
  mySerial = new SomeSerial(hardSerial);
};

GoPowerInverter::GoPowerInverter(uint8_t rx, uint8_t tx) {
  mySerial = new SomeSerial(rx, tx);
};

void GoPowerInverter::begin(int speed) {
  mySerial->begin(19200);
};

// Found the following commands for SD1500
// *RST    // reset
// Power 0 // power OFF
// Power 1 // power ON



void  GoPowerInverter::powerOn() {
  mySerial->println("Power 1");
};

void GoPowerInverter::powerOff() {
  mySerial->println("Power 0");
};

float GoPowerInverter::getVBAT() {

  mySerial->println("VBAT?");
  String RespString = mySerial->readStringUntil("\r\n");
  Serial.print("Response: ");Serial.println(RespString);
  String RespCode = mySerial->readStringUntil("\r\n");
  return RespString.toFloat();
};

