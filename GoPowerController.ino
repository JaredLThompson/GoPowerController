#include <SoftwareSerial.h>

SoftwareSerial mySerial(12,13); //RX, TX

#include "GoPowerInverter.h"

enum errCodes {
  success,
  unknown,
  error,
  undefined
};

struct invResponse {
  float returnValue;
  enum errCodes errorCode;
};


const int psPin=5;
const int buttonPin=2;
const int ledPin=3;

invResponse invResp;

//GoPowerInverter myGPI(12,13); //TX.RX

void setup() {
  pinMode(buttonPin, INPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(psPin, OUTPUT);
  digitalWrite(psPin, HIGH);
  
  Serial.begin(115200);
  digitalWrite(ledPin, HIGH);

  //myGPI.begin(19200);
  
  
  mySerial.begin(19200);
  Serial.println();
  Serial.println("Starting...");
  



  while(digitalRead(buttonPin))
  {
    delay(100);
  }
  delay(100); //wait for any debounce

  blinkLED(4);  

  
  //digitalWrite(ledPin, LOW);
  
}


void blinkLED(int numTimes) {
  int delayTime=150;
  for(int i=0;i<numTimes;i++) {
    digitalWrite(ledPin, LOW);
    delay(delayTime);
    digitalWrite(ledPin, HIGH);
    delay(delayTime);
  }
}

/*
void loop() {
  float voltage = myGPI.getVBAT();
  Serial.print("VBAT: ");
  Serial.println(voltage);
  delay(1000);
  myGPI.powerOff();
}
*/

void loop() {
  // put your main code here, to run repeatedly:
  if(digitalRead(buttonPin))
  {
    //digitalWrite(psPin, LOW); //Turn off the microcontroller
  }

  
  queryInvValue(&invResp, "VBAT");
  Serial.print("Battery Voltage: ");
  Serial.println(invResp.returnValue);
  //SerialPrintError(&invResp);

  queryInvValue(&invResp, "VINV");
  Serial.print("Inverter Voltage: ");
  Serial.println(invResp.returnValue);

  if(invResp.returnValue<10) {
    Serial.print("Powering on...");
    sendInvCommand(&invResp, "POWER 1");
    SerialPrintError(&invResp);
  }
  
  queryInvValue(&invResp, "FRQ");
  Serial.print("Frequency: ");
  Serial.println(invResp.returnValue);
  
  queryInvValue(&invResp, "IINV");
  Serial.print("Inverter Current: ");
  Serial.println(invResp.returnValue);

  queryInvValue(&invResp, "PINV");
  Serial.print("Inverter Power: ");
  Serial.println(invResp.returnValue);

  
  Serial.println("=======================");

  delay(1000);

}

void SerialPrintError(invResponse *resp)
{
  switch(resp->errorCode)
  {
    case success:
      Serial.println("Success!");
      break;
    case unknown:
      Serial.println("Unknown!");
      break;
    case error:
      Serial.println("Error!");
      break;
  }
}

void sendInvCommand(invResponse *resp, String Command)
{
  String respVal = "-1";
  String errCode = "00";
  mySerial.println(Command);
  
  errCode = mySerial.readStringUntil('\n'); //Flush out error code waiting in buffer
  errCode.trim();
  switch(errCode.charAt(0))
  {
    case '=':
      resp->errorCode=success;
      break;
    case '?':
      resp->errorCode=unknown;
      break;
    case '!':
      resp->errorCode=error;
      break;
    default:
      resp->errorCode = undefined;
      break;
  }
}

void queryInvValue(invResponse *resp, String Verb)
{
  String respVal = "-1";
  String errCode = "00";
  mySerial.print(Verb);mySerial.println("?");
  
  respVal = mySerial.readStringUntil('\n');
  respVal.trim();
  resp->returnValue = respVal.toFloat();
  
  errCode = mySerial.readStringUntil('\n'); //Flush out error code waiting in buffer
  errCode.trim();
  switch(errCode.charAt(0))
  {
    case '=':
      resp->errorCode=success;
      break;
    case '?':
      resp->errorCode=unknown;
      break;
    case '!':
      resp->errorCode=error;
      break;
    default:
      resp->errorCode = undefined;
      break;
  }
}



